#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PingBall/RhythmTypes.h"
#include "RhythmChartParser.generated.h"

class FJsonObject;

/**
 * Blueprint function library for parsing JSON MIDI / rhythm chart files into game-ready structs.
 */
UCLASS()
class PINGBALL_API URhythmChartParser : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Parses a chart file using a custom MIDI-to-Lane mapping.
	 * @param RelativeFileName File name or relative path inside Content/Data/
	 * @param MidiToLaneMap Mapping of MIDI note numbers to lane indices
	 * @param OutBeatMap Output array of parsed note structs (sorted chronologically)
	 * @param OutBPM BPM read from chart header
	 * @param OutTimeSignature Time signature numerator read from chart header
	 * @return True if parsing succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Parser")
	static bool ParseChartWithMapping(
		const FString& RelativeFileName,
		const TMap<int32, int32>& MidiToLaneMap,
		TArray<FBeatMapNote>& OutBeatMap,
		float& OutBPM,
		int32& OutTimeSignature
	);

	/**
	 * Legacy/Convenience overload using default MIDI mappings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Parser")
	static bool ParseChart(
		const FString& RelativeFileName,
		TArray<FBeatMapNote>& OutBeatMap,
		float& OutBPM,
		int32& OutTimeSignature
	);

private:
	static void ExtractNotesFromTracks(
		TSharedPtr<FJsonObject> RootObject,
		const TMap<int32, int32>& MidiToLaneMap,
		float BPM,
		TArray<FBeatMapNote>& OutBeatMap
	);
};