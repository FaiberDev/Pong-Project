// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "../GameMode/BaseGameMode.h"
#include "NiagaraComponent.h"
#include "PingBall/Player/ARythmPaddle.h"
// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = ballMesh;
	
	ImpactSquishScale = FVector(0.2f, 0.2f, 0.2f); // Shrinks to 20% of its original size
	ScaleInterpSpeed = 20.0f; // High number = snappy and fast interpolation
	
	// --- NEW: Create the Niagara Component ---
	HitEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffectComponent"));
	HitEffectComponent->SetupAttachment(RootComponent);
    
	// Keep it turned off until the ball is actually
	HitEffectComponent->SetAutoActivate(false);

}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{

	Super::BeginPlay();

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	
	// Save the original size so the math knows where it started
	DefaultScale = GetActorScale3D();
	TargetScale = DefaultScale;
    	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!RhythmStore) return;

	// 1. ALWAYS DO THE SHRINKING MATH
	FVector CurrentScale = GetActorScale3D();
	if (!CurrentScale.Equals(TargetScale, 0.01f))
	{
		FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaTime, ScaleInterpSpeed);
		SetActorScale3D(NewScale);
	}

	// 2. ONLY MOVE IF WE HAVEN'T BEEN HIT YET
	if (!bWasHit)
	{
		float CurrentBeat = RhythmStore->GetCurrentBeat();
		float RawAlpha = (CurrentBeat - SpawnBeat) / (TargetBeat - SpawnBeat);
		float Alpha = FMath::Max(0.0f, RawAlpha);
        
		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		SetActorLocation(NewLocation);

		// Natural destruction if the player completely misses and it flies off screen
		if (Alpha >= 1.2f)
		{
			UE_LOG(LogTemp, Warning, TEXT("PASSIVE MISS! The ball was ignored."));
			ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				// If the ball wasn't hit by the paddle, it's a miss!
				if (!bWasHit) 
				{
					GameMode->RegisterHit(EHitQuality::Miss);
				}
			}
			Destroy();
		}
	}
	else
	{

	}
}



void ABall::TriggerPerfectHitSquish(EHitQuality HitType)
{
	// Shrink and freeze the ball
	TargetScale = FVector(0.0f, 0.0f, 0.0f);
	bWasHit = true;

	// --- NEW: Turn off collision so the paddle ignores it! ---
	SetActorEnableCollision(false);

	// SHOUT TO THE BLUEPRINT: "I was hit, and here is the quality!"
	OnBallHit.Broadcast(HitType);
	UE_LOG(LogTemp, Log, TEXT("Ball was hit! Quality: %d"), static_cast<int32>(HitType));

	SetLifeSpan(1.0f); 
}