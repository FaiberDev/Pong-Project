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
	for (FBeatMapNote CurrentNote : BeatMap)
	{
		if (CurrentNote.BeatToSpawn == BeatNumber)
		{
			if (BallClass)
			{	
				FTransform ThrowerTransform = Throwers[CurrentNote.ChosenThrower] * GetActorTransform();
			
				ABall* SpawnedBall = Cast<ABall>(GetWorld()->SpawnActor<AActor>(BallClass, ThrowerTransform));
				SpawnedBall->InitializeBall(ThrowerTransform.GetLocation(), GetActorLocation(), CurrentAbsoluteBeat, CurrentAbsoluteBeat + LookAheadOffSet);
				
				UE_LOG( LogTemp, Warning, TEXT("Target Beat: %d Spawned at %d" ), BeatNumber, CurrentAbsoluteBeat);
				

			}
		}
	}
}

