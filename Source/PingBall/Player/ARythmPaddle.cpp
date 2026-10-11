// Fill out your copyright notice in the Description page of Project Settings.


#include "ARythmPaddle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Ball/Ball.h"
#include "InputMappingContext.h"


// Sets default values
AARythmPaddle::AARythmPaddle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Create the paddle mesh component and set it as the root component
	paddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleMesh"));
	RootComponent = paddleMesh;
	paddleSpeed = 300.0f; // Default speed for paddle movement
	
	// Create and attach the Strike Zone
	StrikeZone = CreateDefaultSubobject<UBoxComponent>(TEXT("StrikeZone"));
	StrikeZone->SetupAttachment(RootComponent);
	
	// Adjust this size in the Editor to match how forgiving you want the hit area to be!
	StrikeZone->SetBoxExtent(FVector(100.0f, 250.0f, 100.0f));
	
	Color0 = FColor(255, 255, 255, 1).ReinterpretAsLinear(); // White
	Color1 = FColor(229, 72, 103, 1).ReinterpretAsLinear(); // Red
	Color2 = FColor(183, 246,105, 1).ReinterpretAsLinear(); //Green
	Color3 = FColor(84,158,250, 1).ReinterpretAsLinear(); //Blue
	


}

// Called when the game starts or when spawned
void AARythmPaddle::BeginPlay()
{
	Super::BeginPlay();
	//Set input context 
	SetInputMappingContext();
	CreateAndSetMaterialInstanceDynamic(0);
	ChangePaddleColor(Color0);
	
	GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());

	
	
}
// Called every frame
void AARythmPaddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Called to bind functionality to input
void AARythmPaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast the default component to UE5's UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind the action to our movement function
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARythmPaddle::OnMoveInput);
		EnhancedInputComp->BindAction(Color1_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor1Input);
		EnhancedInputComp->BindAction(Color2_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor2Input);
		EnhancedInputComp->BindAction(Color3_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor3Input);
	}
}

void AARythmPaddle::MovePaddleHorizontal(float AxisValue, float DeltaTime)
{
	if (AxisValue != 0.0f)
	{
		FVector Movement = FVector(0.0f, AxisValue * paddleSpeed * DeltaTime, 0.0f);
		AddActorLocalOffset(Movement, true);
		
	}
}
void AARythmPaddle::OnMoveInput(const FInputActionValue& Value)
{
	// Extract the scalar float value from the input action
	float AxisValue = Value.Get<float>();

	// Pass the input value and the frame delta time into your movement logic
	MovePaddleHorizontal(AxisValue, GetWorld()->GetDeltaSeconds());
}

void AARythmPaddle::SetInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add the input mapping context
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
void AARythmPaddle::AttemptHit()
{
    if (!bCanHit) return;
    
    URhythmStoreSubsystem* RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
    if (!RhythmStore) return;

    TArray<AActor*> OverlappingBalls;
    StrikeZone->GetOverlappingActors(OverlappingBalls, ABall::StaticClass());
    
    ABall* BestBall = nullptr;
    float SmallestOffset = 9999.0f; 
    float CurrentBeat = RhythmStore->GetCurrentBeat();

    // 1. LOOP THROUGH EVERY BALL IN THE STRIKE ZONE
    for (AActor* Actor : OverlappingBalls)
    {
       ABall* Ball = Cast<ABall>(Actor);
       if (Ball)
       {
          // 2. Only check balls that match the color AND haven't been hit yet
          if (Ball->BallColorIndex == CurrentColorIndex && !Ball->bWasHit)
          {
             float Offset = FMath::Abs(Ball->TargetBeat - CurrentBeat);
             
             // 3. Find the one closest to the perfect beat
             if (Offset < SmallestOffset)
             {
                SmallestOffset = Offset;
                BestBall = Ball;
             }
          }
       }
    }

    // 4. IF WE FOUND A MATCHING BALL, SCORE IT! (Cascading from tightest to widest)
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
          // The ball was physically touching the very edge of the strike zone, 
          // but the timing was outside even the "Normal" window.
          if (GameMode) GameMode->RegisterHit(EHitQuality::Miss);
          UE_LOG(LogTemp, Warning, TEXT("ACTIVE MISS! Button pressed, but timing was way off."));
       }
    }
    else
    {
       // The player pressed a color, but no balls of that color were in the zone
       if (GameMode) GameMode->RegisterHit(EHitQuality::Miss);
       UE_LOG(LogTemp, Error, TEXT("WRONG COLOR OR EMPTY AIR! Paddle was %d"), CurrentColorIndex);
    }
    
    // 5. LOCK THE PADDLE FOR ANTI-MASHING
    bCanHit = false;
    GetWorldTimerManager().SetTimer(HitCooldownTimer, this, &AARythmPaddle::ResetHitCooldown, 0.05f, false);
}

// Change the paddle color based on the input value
void AARythmPaddle::CreateAndSetMaterialInstanceDynamic(int32 ElementIndex)
{
	DynamicMaterial = paddleMesh->CreateAndSetMaterialInstanceDynamic(ElementIndex);
}

// Change the paddle color based on the input value
void AARythmPaddle::ChangePaddleColor(FLinearColor NewColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Color", NewColor);
	}
}

void AARythmPaddle::OnColor1Input(const FInputActionValue& Value)
{
	ChangePaddleColor(Color1);
	CurrentColorIndex = 1; // Red
	AttemptHit(); 
}

void AARythmPaddle::OnColor2Input(const FInputActionValue& Value)
{
	ChangePaddleColor(Color2);
	CurrentColorIndex = 2; // Green
	AttemptHit(); 
}

void AARythmPaddle::OnColor3Input(const FInputActionValue& Value)
{
	ChangePaddleColor(Color3);
	CurrentColorIndex = 3; // Blue
	AttemptHit(); 
}

void AARythmPaddle::ResetHitCooldown()
{
	// Unlock the paddle so the player can press buttons again
	bCanHit = true;
}