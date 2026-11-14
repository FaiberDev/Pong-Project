#pragma once

#include "CoreMinimal.h"
#include "RhythmTypes.generated.h"

/**
 * Accuracy tier for hitting a note.
 */
UENUM(BlueprintType)
enum class EHitQuality : uint8
{
	Max     UMETA(DisplayName = "Max"),     // Highest tier (e.g. Rainbow 300)
	Perfect UMETA(DisplayName = "Perfect"), // Perfect tier (300)
	Great   UMETA(DisplayName = "Great"),   // Great tier (200)
	Good    UMETA(DisplayName = "Good"),    // Good tier (100)
	Normal  UMETA(DisplayName = "Normal"),  // Normal tier (50)
	Miss    UMETA(DisplayName = "Miss")     // Missed / empty hit (0)
};

/**
 * Represents a single note parsed from chart/MIDI data.
 */
USTRUCT(BlueprintType)
struct FBeatMapNote
{
	GENERATED_BODY()

	// The beat number at which the note should be spawned (for lookahead)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|BeatMap")
	int32 BeatToSpawn = 0;

	// Exact fractional beat at which this note crosses the target hit line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|BeatMap")
	float ExactHitBeat = 0.0f;

	// The index of the thrower/lane that spawns this note
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|BeatMap")
	int32 ChosenThrower = 0;

	// Target color index (1 = Red, 2 = Green, 3 = Blue, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|BeatMap")
	int32 NoteColorIndex = 1;

	// Original timestamp in audio file (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|BeatMap")
	float TimeInSeconds = 0.0f;
};

/**
 * Designer-configurable scoring settings for hit accuracy tiers.
 */
USTRUCT(BlueprintType)
struct FRhythmTierScoreData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Scoring")
	int32 AccuracyPoints = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Scoring")
	float AccuracyWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Scoring")
	float MultiplierDelta = 0.0f;
};

/**
 * Standard palette definition so colors aren't duplicated across classes.
 */
USTRUCT(BlueprintType)
struct FRhythmColorPalette
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Colors")
	FLinearColor DefaultColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // White

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Colors")
	FLinearColor LaneColor1 = FLinearColor(0.898f, 0.282f, 0.404f, 1.0f); // Red / Pink (229, 72, 103)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Colors")
	FLinearColor LaneColor2 = FLinearColor(0.718f, 0.965f, 0.412f, 1.0f); // Green (183, 246, 105)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Colors")
	FLinearColor LaneColor3 = FLinearColor(0.329f, 0.620f, 0.980f, 1.0f); // Blue (84, 158, 250)

	FLinearColor GetColorByIndex(int32 Index) const
	{
		switch (Index)
		{
		case 1: return LaneColor1;
		case 2: return LaneColor2;
		case 3: return LaneColor3;
		default: return DefaultColor;
		}
	}
};
