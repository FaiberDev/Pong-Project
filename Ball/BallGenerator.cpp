// Fill out your copyright notice in the Description page of Project Settings.


#include "BallGenerator.h"
#include "Components/SceneComponent.h"
#include "../RhythmStoreSubsystem.h"

// Sets default values
ABallGenerator::ABallGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Create Throwers
	for (int i = 0; i <NumberOfTowers; ++i)
	{
		// Create a unique name for each thrower component
		FName ComponentName = FName(*FString::Printf(TEXT("Thrower_%d"), i));
    	
		// Create a new scene component for the thrower and attach it to the root component
		USceneComponent* NewThrower = CreateDefaultSubobject<USceneComponent>(ComponentName);
		Throwers.Add(NewThrower);
		NewThrower->SetupAttachment(RootComponent);
	}



	
	
}

// Called when the game starts or when spawned
void ABallGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	
	// Get the RhythmStoreSubsystem and bind to the OnBeat event
	//URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	//if (Store)
	//{
	//	Store->OnBeat.AddDynamic(this, &ABallGenerator::GeatBeatFromStore);
	//}
	
}

// Called every frame
void ABallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABallGenerator::LookAheadForNextBeat(int32 CurrentBeatNumber)
{
	
}

