// Fill out your copyright notice in the Description page of Project Settings.


#include "BallGenerator.h"
#include "Components/SceneComponent.h"
#include "../RhythmStoreSubsystem.h"
#include "Ball.h"
// Sets default values
ABallGenerator::ABallGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
	
}

// Called when the game starts or when spawned
void ABallGenerator::BeginPlay()
{
	Super::BeginPlay();
	// Get the RhythmStoreSubsystem and bind to its OnBeat event
	URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	if (Store)
	{
		Store->OnBeat.AddDynamic(this, &ABallGenerator::GetBeatFromStore);
	}
	
}

// Called every frame
void ABallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABallGenerator::GetBeatFromStore(int32 BeatNumber)
{	
	
	CurrentAbsoluteBeat++;
	
	int32 TargetBeatNumber = CurrentAbsoluteBeat + LookAheadOffSet; // Look ahead 3 beats
	
	if (TargetBeatNumber >= 1)
	{
		LookAheadForNextBeat(TargetBeatNumber);	
	}
	
}

void ABallGenerator::LookAheadForNextBeat(int32 BeatNumber)
{
	
	float PaddleXLocation = 0.0f;
	for (FBeatMapNote CurrentNote : BeatMap)
	{
		if (CurrentNote.BeatToSpawn == BeatNumber)
		{
			if (BallClass)
			{  
				// 1. Get the exact starting transform for this specific lane
				FTransform ThrowerTransform = Throwers[CurrentNote.ChosenThrower] * GetActorTransform();
				FVector StartLocation = ThrowerTransform.GetLocation();
             
				// 2. Spawn the ball
				ABall* SpawnedBall = Cast<ABall>(GetWorld()->SpawnActor<AActor>(BallClass, ThrowerTransform));

				if (SpawnedBall)
				{
					FVector TargetLocation = FVector(PaddleXLocation, StartLocation.Y, 60.0f);

					// 1. Grab the exact floating-point beat from the Store
					URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
					float ExactSpawnBeat = Store ? Store->GetCurrentBeat() : (float)CurrentAbsoluteBeat;

					// 2. Give the ball the ExactSpawnBeat instead of CurrentAbsoluteBeat
					SpawnedBall->InitializeBall(StartLocation, TargetLocation, ExactSpawnBeat, ExactSpawnBeat + LookAheadOffSet);
                 
				//	UE_LOG(LogTemp, Warning, TEXT("Target Beat: %d Spawned at %d. Thrower Index: %d, Target Location: %s"), 
				//	   BeatNumber, 
			//		   CurrentAbsoluteBeat, 
			//		   CurrentNote.ChosenThrower, 
			//		   *TargetLocation.ToString()
			//		);          
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn ball at beat %d. Thrower Index: %d"), BeatNumber, CurrentNote.ChosenThrower);
				}
			}
		}
	}
}