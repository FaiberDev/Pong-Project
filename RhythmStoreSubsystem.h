// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RhythmStoreSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatEvent, int32, BeatNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBPMSet, float, NewBPM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatsPerBarSet, int32, NewBeatsPerBar);

UCLASS()
class PINGBALL_API URhythmStoreSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	
public:
	//  Shared Rhythm Data
	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Store")
	float CurrentBPM = 120.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Store")
	int32 BeatsPerBar = 8;

	// Event broadcast when a beat occurs
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store")
	FOnBeatEvent OnBeat;	
	
	// Event broadcast when BPM is set
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store")
	FOnBPMSet OnBPMSet;
	
	// Function to set the BPM and broadcast the event
	void SetBPM(float NewBPM);

	// Event broadcast when BeatsPerBar is set
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store")
	FOnBeatsPerBarSet OnBeatsPerBarSet;
	
	// Function to get the current BPM
	float GetBPM() const { return CurrentBPM; }	
};
