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
	
	
	float CurrentBeat = 0.0f;
	
	
	//Point System
	
public:
	//  Shared Rhythm Data
	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Store")
	float CurrentBPM = 146.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Store")
	int32 TimeSignature = 4;

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
	
	// Setters and getters
	float GetBPM() const { return CurrentBPM; }
	
	int32 GetTimeSignature() const { return TimeSignature; }
	void SetTimeSignature(float inTimeSignature){TimeSignature = inTimeSignature; OnBeatsPerBarSet.Broadcast(TimeSignature);};
	
	float GetCurrentBeat() const { return CurrentBeat; }
	void SetCurrentBeat(float InBeat) { CurrentBeat = InBeat; }
};
