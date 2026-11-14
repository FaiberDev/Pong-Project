#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PingBall/RhythmTypes.h"
#include "RhythmStoreSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatEvent, int32, BeatNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBPMSet, float, NewBPM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatsPerBarSet, int32, NewBeatsPerBar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSongStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSongFinishedSignature);

/**
 * World subsystem acting as the single source of truth for audio timing,
 * BPM, time signature, visual color palette, and audio latency calibration.
 */
UCLASS()
class PINGBALL_API URhythmStoreSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Shared Rhythm Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm Store|Timing")
	float CurrentBPM = 146.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm Store|Timing")
	int32 TimeSignature = 4;

	// Exact floating-point beat continuously updated from Quartz audio clock
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm Store|Timing")
	float CurrentBeat = 0.0f;

	// User audio/display calibration offset in milliseconds
	// Compensates for bluetooth latency, TV display processing, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Store|Calibration")
	float AudioOffsetMs = 0.0f;

	// Central color palette shared across Paddle, Balls, and UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Store|Colors")
	FRhythmColorPalette ColorPalette;

	// Event broadcast when an integer/quantized beat occurs
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store|Events")
	FOnBeatEvent OnBeat;

	// Event broadcast when BPM changes
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store|Events")
	FOnBPMSet OnBPMSet;

	// Event broadcast when time signature changes
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store|Events")
	FOnBeatsPerBarSet OnBeatsPerBarSet;

	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store|Events")
	FOnSongStartedSignature OnSongStarted;

	UPROPERTY(BlueprintAssignable, Category = "Rhythm Store|Events")
	FOnSongFinishedSignature OnSongFinished;

	// --- Timing Functions ---
	UFUNCTION(BlueprintCallable, Category = "Rhythm Store|Timing")
	void SetBPM(float NewBPM);

	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Timing")
	float GetBPM() const { return CurrentBPM; }

	UFUNCTION(BlueprintCallable, Category = "Rhythm Store|Timing")
	void SetTimeSignature(int32 InTimeSignature);

	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Timing")
	int32 GetTimeSignature() const { return TimeSignature; }

	UFUNCTION(BlueprintCallable, Category = "Rhythm Store|Timing")
	void SetCurrentBeat(float InBeat);

	// Returns the current continuous beat, factoring in player audio offset calibration
	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Timing")
	float GetCurrentBeat() const;

	// Returns the raw beat before calibration offset
	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Timing")
	float GetRawBeat() const { return CurrentBeat; }

	// --- Latency Calibration ---
	UFUNCTION(BlueprintCallable, Category = "Rhythm Store|Calibration")
	void SetAudioOffsetMs(float InOffsetMs) { AudioOffsetMs = InOffsetMs; }

	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Calibration")
	float GetAudioOffsetMs() const { return AudioOffsetMs; }

	// --- Visuals & Color ---
	UFUNCTION(BlueprintPure, Category = "Rhythm Store|Colors")
	FLinearColor GetColorForIndex(int32 Index) const { return ColorPalette.GetColorByIndex(Index); }
};
