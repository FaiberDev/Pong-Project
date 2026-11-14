#include "ARythmPaddle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Ball/Ball.h"
#include "PingBall/GameMode/BaseGameMode.h"
#include "InputMappingContext.h"
#include "TimerManager.h"

AARythmPaddle::AARythmPaddle()
{
	PrimaryActorTick.bCanEverTick = false;

	paddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleMesh"));
	RootComponent = paddleMesh;
	paddleSpeed = 300.0f;

	StrikeZone = CreateDefaultSubobject<UBoxComponent>(TEXT("StrikeZone"));
	StrikeZone->SetupAttachment(RootComponent);
	StrikeZone->SetBoxExtent(FVector(100.0f, 250.0f, 100.0f));

	Color0 = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Color1 = FLinearColor(0.898f, 0.282f, 0.404f, 1.0f);
	Color2 = FLinearColor(0.718f, 0.965f, 0.412f, 1.0f);
	Color3 = FLinearColor(0.329f, 0.620f, 0.980f, 1.0f);
}

void AARythmPaddle::BeginPlay()
{
	Super::BeginPlay();

	SetInputMappingContext();
	CreateAndSetMaterialInstanceDynamic(0);

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	if (RhythmStore)
	{
		// Pull centralized palette colors if configured
		Color0 = RhythmStore->GetColorForIndex(0);
		Color1 = RhythmStore->GetColorForIndex(1);
		Color2 = RhythmStore->GetColorForIndex(2);
		Color3 = RhythmStore->GetColorForIndex(3);
	}

	ChangePaddleColor(Color0);
	GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
}

void AARythmPaddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARythmPaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARythmPaddle::OnMoveInput);
		}
		if (Color1_Action)
		{
			EnhancedInputComp->BindAction(Color1_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor1Input);
		}
		if (Color2_Action)
		{
			EnhancedInputComp->BindAction(Color2_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor2Input);
		}
		if (Color3_Action)
		{
			EnhancedInputComp->BindAction(Color3_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor3Input);
		}
	}
}

void AARythmPaddle::MovePaddleHorizontal(float AxisValue, float DeltaTime)
{
	if (!FMath::IsNearlyZero(AxisValue))
	{
		const FVector Movement = FVector(0.0f, AxisValue * paddleSpeed * DeltaTime, 0.0f);
		AddActorLocalOffset(Movement, true);
	}
}

void AARythmPaddle::OnMoveInput(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	MovePaddleHorizontal(AxisValue, GetWorld()->GetDeltaSeconds());
}

void AARythmPaddle::SetInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AARythmPaddle::HandleColorStrike(int32 ColorIndex)
{
	CurrentColorIndex = ColorIndex;

	switch (ColorIndex)
	{
	case 1: ChangePaddleColor(Color1); break;
	case 2: ChangePaddleColor(Color2); break;
	case 3: ChangePaddleColor(Color3); break;
	default: ChangePaddleColor(Color0); break;
	}

	AttemptHit();
}

void AARythmPaddle::OnColor1Input(const FInputActionValue& Value)
{
	HandleColorStrike(1);
}

void AARythmPaddle::OnColor2Input(const FInputActionValue& Value)
{
	HandleColorStrike(2);
}

void AARythmPaddle::OnColor3Input(const FInputActionValue& Value)
{
	HandleColorStrike(3);
}

void AARythmPaddle::AttemptHit()
{
	if (!bCanHit)
	{
		return;
	}

	if (!RhythmStore)
	{
		RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
		if (!RhythmStore) return;
	}

	TArray<AActor*> OverlappingBalls;
	StrikeZone->GetOverlappingActors(OverlappingBalls, ABall::StaticClass());

	ABall* BestBall = nullptr;
	float SmallestOffset = MAX_FLT;
	const float CurrentBeat = RhythmStore->GetCurrentBeat();

	// 1. Find the matching ball with the closest timing to the strike plane
	for (AActor* Actor : OverlappingBalls)
	{
		ABall* Ball = Cast<ABall>(Actor);
		if (Ball && !Ball->bWasHit && Ball->bIsActiveInPool)
		{
			if (Ball->BallColorIndex == CurrentColorIndex)
			{
				const float Offset = FMath::Abs(Ball->TargetBeat - CurrentBeat);
				if (Offset < SmallestOffset)
				{
					SmallestOffset = Offset;
					BestBall = Ball;
				}
			}
		}
	}

	// 2. Score hit based on accuracy window
	if (BestBall)
	{
		if (SmallestOffset <= MaxWindow)
		{
			BestBall->TriggerPerfectHitSquish(EHitQuality::Max);
			if (GameMode) GameMode->RegisterHit(EHitQuality::Max);
			TriggerHitGlow(EHitQuality::Max);
		}
		else if (SmallestOffset <= PerfectWindow)
		{
			BestBall->TriggerPerfectHitSquish(EHitQuality::Perfect);
			if (GameMode) GameMode->RegisterHit(EHitQuality::Perfect);
			TriggerHitGlow(EHitQuality::Perfect);
		}
		else if (SmallestOffset <= GreatWindow)
		{
			BestBall->TriggerPerfectHitSquish(EHitQuality::Great);
			if (GameMode) GameMode->RegisterHit(EHitQuality::Great);
			TriggerHitGlow(EHitQuality::Great);
		}
		else if (SmallestOffset <= GoodWindow)
		{
			BestBall->TriggerPerfectHitSquish(EHitQuality::Good);
			if (GameMode) GameMode->RegisterHit(EHitQuality::Good);
			TriggerHitGlow(EHitQuality::Good);
		}
		else if (SmallestOffset <= NormalWindow)
		{
			BestBall->TriggerPerfectHitSquish(EHitQuality::Normal);
			if (GameMode) GameMode->RegisterHit(EHitQuality::Normal);
			TriggerHitGlow(EHitQuality::Normal);
		}
		else
		{
			// Note is in strike zone physical volume but outside any valid timing window
			if (GameMode) GameMode->RegisterHit(EHitQuality::Miss);
		}
	}
	else
	{
		// Empty air or wrong color pressed
		if (GameMode) GameMode->RegisterHit(EHitQuality::Miss);
	}

	// 3. Apply anti-mash cooldown
	bCanHit = false;
	GetWorldTimerManager().SetTimer(HitCooldownTimer, this, &AARythmPaddle::ResetHitCooldown, HitCooldownDuration, false);
}

void AARythmPaddle::CreateAndSetMaterialInstanceDynamic(int32 ElementIndex)
{
	if (paddleMesh)
	{
		DynamicMaterial = paddleMesh->CreateAndSetMaterialInstanceDynamic(ElementIndex);
	}
}

void AARythmPaddle::ChangePaddleColor(FLinearColor NewColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), NewColor);
	}
}

void AARythmPaddle::ResetHitCooldown()
{
	bCanHit = true;
}