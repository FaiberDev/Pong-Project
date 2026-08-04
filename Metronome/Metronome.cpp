// Fill out your copyright notice in the Description page of Project Settings.


#include "Metronome.h"
#include "../RhythmStoreSubsystem.h"

// Sets default values
AMetronome::AMetronome()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMetronome::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the RhythmStoreSubsystem and bind to the OnBeat event
    URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	if (Store)
	{
		Store->OnBeat.AddDynamic(this, &AMetronome::GetBeatFromStore);
	}

}

// Called every frame
void AMetronome::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMetronome::GetBeatFromStore(int32 BeatNumber)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Metronome received beat: %d"), BeatNumber));
}

