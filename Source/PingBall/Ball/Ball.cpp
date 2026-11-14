#include "Ball.h"
#include "PingBall/GameMode/BaseGameMode.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = ballMesh;

	ImpactSquishScale = FVector(0.2f, 0.2f, 0.2f);
	ScaleInterpSpeed = 20.0f;

	HitEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffectComponent"));
	HitEffectComponent->SetupAttachment(RootComponent);
	HitEffectComponent->SetAutoActivate(false);

	DefaultScale = FVector::OneVector;
	TargetScale = FVector::OneVector;
}

void ABall::BeginPlay()
{
	Super::BeginPlay();

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	DefaultScale = GetActorScale3D();
	TargetScale = DefaultScale;
}

void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActiveInPool)
	{
		return;
	}

	// 1. Smoothly interpolate squish scaling on hit
	FVector CurrentScale = GetActorScale3D();
	if (!CurrentScale.Equals(TargetScale, 0.01f))
	{
		FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaTime, ScaleInterpSpeed);
		SetActorScale3D(NewScale);
	}

	// 2. Linear motion along the rhythm lane while active and not yet hit
	if (!bWasHit && RhythmStore)
	{
		float CurrentBeat = RhythmStore->GetCurrentBeat();
		float Denominator = TargetBeat - SpawnBeat;
		float Alpha = (Denominator > 0.0001f) ? (CurrentBeat - SpawnBeat) / Denominator : 1.0f;
		Alpha = FMath::Max(0.0f, Alpha);

		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		SetActorLocation(NewLocation);

		// Natural miss check when the note passes beyond the strike zone
		if (Alpha >= 1.2f)
		{
			ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode && !bWasHit)
			{
				GameMode->RegisterHit(EHitQuality::Miss);
			}

			DeactivateBall();
		}
	}
}

void ABall::InitializeBall(
	const FVector& InStartLocation,
	const FVector& InTargetLocation,
	float InSpawnBeat,
	float InTargetBeat,
	int32 InColorIndex)
{
	StartLocation = InStartLocation;
	TargetLocation = InTargetLocation;
	SpawnBeat = InSpawnBeat;
	TargetBeat = InTargetBeat;
	bWasHit = false;

	if (!RhythmStore)
	{
		RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	}

	// Reset scale & state
	SetActorLocation(StartLocation);
	SetActorScale3D(DefaultScale);
	TargetScale = DefaultScale;
	SetActorEnableCollision(true);
	SetPoolActive(true);

	// Resolve ball color
	if (InColorIndex > 0)
	{
		BallColorIndex = InColorIndex;
	}
	else
	{
		BallColorIndex = FMath::RandRange(1, 3);
	}

	// Reuse or create dynamic material (avoids reallocating on every pooled spawn)
	if (!DynamicMaterial && ballMesh)
	{
		DynamicMaterial = ballMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (DynamicMaterial)
	{
		FLinearColor ChosenColor = FLinearColor::White;
		if (RhythmStore)
		{
			ChosenColor = RhythmStore->GetColorForIndex(BallColorIndex);
		}
		else
		{
			// Safe fallback
			switch (BallColorIndex)
			{
			case 1: ChosenColor = FLinearColor(0.898f, 0.282f, 0.404f, 1.0f); break;
			case 2: ChosenColor = FLinearColor(0.718f, 0.965f, 0.412f, 1.0f); break;
			case 3: ChosenColor = FLinearColor(0.329f, 0.620f, 0.980f, 1.0f); break;
			default: ChosenColor = FLinearColor::White; break;
			}
		}
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), ChosenColor);
	}

	if (HitEffectComponent)
	{
		HitEffectComponent->Deactivate();
	}
}

void ABall::TriggerPerfectHitSquish(EHitQuality HitType)
{
	TargetScale = FVector::ZeroVector;
	bWasHit = true;
	SetActorEnableCollision(false);

	if (HitEffectComponent)
	{
		HitEffectComponent->Activate(true);
	}

	OnBallHit.Broadcast(HitType);

	// Recycle after squish animation finishes
	GetWorldTimerManager().ClearTimer(DespawnTimerHandle);
	GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABall::DeactivateBall, 0.5f, false);
}

void ABall::DeactivateBall()
{
	GetWorldTimerManager().ClearTimer(DespawnTimerHandle);
	SetPoolActive(false);

	// Notify the spawner pool that this ball is free
	if (OnBallRecycle.IsBound())
	{
		OnBallRecycle.Broadcast(this);
	}
	else
	{
		// Fallback if not using object pooling
		Destroy();
	}
}

void ABall::SetPoolActive(bool bActive)
{
	bIsActiveInPool = bActive;
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
	SetActorTickEnabled(bActive);
}