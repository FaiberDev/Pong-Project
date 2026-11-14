#include "RhythmChartParser.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool URhythmChartParser::ParseChart(
	const FString& RelativeFileName,
	TArray<FBeatMapNote>& OutBeatMap,
	float& OutBPM,
	int32& OutTimeSignature)
{
	// Default fallback map: 40->0, 41->1, 43->2
	TMap<int32, int32> DefaultMap;
	DefaultMap.Add(40, 0);
	DefaultMap.Add(41, 1);
	DefaultMap.Add(43, 2);

	return ParseChartWithMapping(RelativeFileName, DefaultMap, OutBeatMap, OutBPM, OutTimeSignature);
}

bool URhythmChartParser::ParseChartWithMapping(
	const FString& RelativeFileName,
	const TMap<int32, int32>& MidiToLaneMap,
	TArray<FBeatMapNote>& OutBeatMap,
	float& OutBPM,
	int32& OutTimeSignature)
{
	FString CleanFileName = RelativeFileName;
	if (CleanFileName.IsEmpty())
	{
		CleanFileName = TEXT("kamisama.json");
	}

	FString FilePath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), CleanFileName);
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[RhythmChartParser] Failed to load chart file from: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[RhythmChartParser] JSON Deserialization failed for: %s"), *FilePath);
		return false;
	}

	// 1. Parse BPM & Time Signature from header
	OutBPM = 120.0f;
	OutTimeSignature = 4;

	const TSharedPtr<FJsonObject>* HeaderObj = nullptr;
	if (RootObject->TryGetObjectField(TEXT("header"), HeaderObj) && HeaderObj && (*HeaderObj).IsValid())
	{
		double ParsedBPM = 120.0;
		if ((*HeaderObj)->TryGetNumberField(TEXT("bpm"), ParsedBPM))
		{
			OutBPM = static_cast<float>(ParsedBPM);
		}

		const TArray<TSharedPtr<FJsonValue>>* TimeSigArray = nullptr;
		if ((*HeaderObj)->TryGetArrayField(TEXT("timeSignature"), TimeSigArray) && TimeSigArray && TimeSigArray->Num() > 0)
		{
			OutTimeSignature = FMath::RoundToInt32((*TimeSigArray)[0]->AsNumber());
		}
	}

	// 2. Extract notes with the mapping
	ExtractNotesFromTracks(RootObject, MidiToLaneMap, OutBPM, OutBeatMap);

	// 3. Sort notes chronologically by hit beat / time to guarantee O(1) streaming
	OutBeatMap.Sort([](const FBeatMapNote& A, const FBeatMapNote& B)
	{
		return A.ExactHitBeat < B.ExactHitBeat;
	});

	UE_LOG(LogTemp, Log, TEXT("[RhythmChartParser] Successfully parsed %d notes from %s (BPM: %.2f, TimeSig: %d/4)"),
		OutBeatMap.Num(), *CleanFileName, OutBPM, OutTimeSignature);

	return true;
}

void URhythmChartParser::ExtractNotesFromTracks(
	TSharedPtr<FJsonObject> RootObject,
	const TMap<int32, int32>& MidiToLaneMap,
	float BPM,
	TArray<FBeatMapNote>& OutBeatMap)
{
	OutBeatMap.Reset();

	const TArray<TSharedPtr<FJsonValue>>* TracksArray = nullptr;
	if (!RootObject->TryGetArrayField(TEXT("tracks"), TracksArray) || !TracksArray)
	{
		return;
	}

	for (const TSharedPtr<FJsonValue>& TrackVal : *TracksArray)
	{
		if (!TrackVal.IsValid()) continue;
		TSharedPtr<FJsonObject> TrackObj = TrackVal->AsObject();
		if (!TrackObj.IsValid()) continue;

		const TArray<TSharedPtr<FJsonValue>>* NotesArray = nullptr;
		if (TrackObj->TryGetArrayField(TEXT("notes"), NotesArray) && NotesArray)
		{
			for (const TSharedPtr<FJsonValue>& NoteVal : *NotesArray)
			{
				if (!NoteVal.IsValid()) continue;
				TSharedPtr<FJsonObject> NoteObj = NoteVal->AsObject();
				if (!NoteObj.IsValid()) continue;

				double TimeInSeconds = 0.0;
				NoteObj->TryGetNumberField(TEXT("time"), TimeInSeconds);

				int32 MidiNote = 0;
				NoteObj->TryGetNumberField(TEXT("midi"), MidiNote);

				// Calculate exact fractional hit beat: Beat = (Time * BPM) / 60
				float ExactHitBeat = static_cast<float>((TimeInSeconds * BPM) / 60.0);
				int32 RoundedBeat = FMath::RoundToInt(ExactHitBeat);

				// Map MIDI note to lane (thrower)
				int32 ThrowerIndex = 0;
				if (const int32* FoundLane = MidiToLaneMap.Find(MidiNote))
				{
					ThrowerIndex = *FoundLane;
				}
				else
				{
					// If note pitch wasn't explicitly mapped, fall back to pitch modulo 3
					ThrowerIndex = FMath::Abs(MidiNote) % 3;
				}

				FBeatMapNote Note;
				Note.BeatToSpawn = RoundedBeat;
				Note.ExactHitBeat = ExactHitBeat;
				Note.ChosenThrower = ThrowerIndex;
				Note.TimeInSeconds = static_cast<float>(TimeInSeconds);
				Note.NoteColorIndex = (ThrowerIndex % 3) + 1; // 1-indexed color matching lane

				OutBeatMap.Add(Note);
			}
		}
	}
}