#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PingBall/RhythmTypes.h"
#include "RhythmSongDataAsset.generated.h"

class USoundWave;

/**
 * Data asset defining all metadata, audio, and chart mapping for a playable rhythm track.
 */
UCLASS(BlueprintType)
class PINGBALL_API URhythmSongDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URhythmSongDataAsset();

	// Display name of the song
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Song Info")
	FText SongTitle;

	// Artist / Producer name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Song Info")
	FText ArtistName;

	// Audio track asset to play
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundWave> SongAudio;

	// Song tempo in BPM (can also be read from chart JSON header)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timing")
	float DefaultBPM = 120.0f;

	// Time signature numerator (e.g. 4 for 4/4)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timing")
	int32 DefaultTimeSignature = 4;

	// Song-specific audio start offset in seconds (compensation for intro silence or audio drift)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timing")
	float AudioOffsetSeconds = 0.0f;

	// Relative path inside Content/Data/ (e.g., "kamisama.json")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	FString ChartJsonPath = TEXT("kamisama.json");

	// Flexible mapping from MIDI note numbers (e.g., 40=E2, 41=F2, 43=G2) to Lane/Thrower index
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	TMap<int32, int32> MidiPitchToLaneMap;

	// Resolves a MIDI pitch to a thrower/lane index, falling back to Lane 0 if unmapped
	UFUNCTION(BlueprintCallable, Category = "Chart")
	int32 GetLaneForMidiPitch(int32 MidiPitch) const;

	// Populates default test values (useful when instantiated in editor)
	virtual void PostInitProperties() override;
};
