#include "RhythmChartParser.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"

bool URhythmChartParser::ParseChart(FString FileName, TArray<FBeatMapNote>& OutBeatMap, float& OutBPM, int32& OutTimeSignature)
{
	FString FilePath = FPaths::ProjectContentDir() + "Data/" + FileName;
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) return false;

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
	{
		//  LOGIC GOES HERE:
		const TSharedPtr<FJsonObject>* HeaderObj;
		if (RootObject->TryGetObjectField(TEXT("header"), HeaderObj))
		{
			(*HeaderObj)->TryGetNumberField(TEXT("bpm"), OutBPM);
        
			const TArray<TSharedPtr<FJsonValue>>* TimeSigArray;
			if ((*HeaderObj)->TryGetArrayField(TEXT("timeSignature"), TimeSigArray))
			{
				if (TimeSigArray->Num() > 0)
				{
					OutTimeSignature = (*TimeSigArray)[0]->AsNumber();
				}
			}
		}
        
		GiveChartToBallGenerator(RootObject,OutBeatMap, OutBPM);  
		
		return true;
	}
    
	return false;
}

void URhythmChartParser::GiveChartToBallGenerator( TSharedPtr<FJsonObject> RootObject, TArray<FBeatMapNote>& OutBeatMap, float OutBPM)
{
	// --- START OF TRACKS LOOP ---
        const TArray<TSharedPtr<FJsonValue>>* TracksArray;
        if (RootObject->TryGetArrayField(TEXT("tracks"), TracksArray))
        {
            OutBeatMap.Empty(); // Make sure we start with a clean slate

            // Loop through every track in the JSON
            for (const auto& TrackVal : *TracksArray)
            {
                TSharedPtr<FJsonObject> TrackObj = TrackVal->AsObject();
                const TArray<TSharedPtr<FJsonValue>>* NotesArray;

                // Check if this track has a "notes" array with actual items inside it
                if (TrackObj->TryGetArrayField(TEXT("notes"), NotesArray) && NotesArray->Num() > 0)
                {
                    // Loop through every single note in this track
                    for (const auto& NoteVal : *NotesArray)
                    {
                        TSharedPtr<FJsonObject> NoteObj = NoteVal->AsObject();

                        // Grab the exact time and MIDI pitch
                        double TimeInSeconds = NoteObj->GetNumberField(TEXT("time"));
                        int32 MidiNote = NoteObj->GetIntegerField(TEXT("midi"));

                        // 1. Calculate the beat number
                        // Formula: (Time * BPM) / 60
                        int32 CalculatedBeat = FMath::RoundToInt((TimeInSeconds * OutBPM) / 60.0);

                        // 2. Map the MIDI note to your Thrower Index
                        int32 ThrowerIndex = 0;
                        switch (MidiNote)
                        {
                            case 40: ThrowerIndex = 0; break; // E2 -> Thrower 0
                            case 41: ThrowerIndex = 1; break; // F2 -> Thrower 1
                            case 43: ThrowerIndex = 2; break; // G2 -> Thrower 2
                            default: ThrowerIndex = 0; break; // Fallback
                        }

                        // 3. Create the struct and add it to our Output array!
                        FBeatMapNote NewNote;
                        NewNote.BeatToSpawn = CalculatedBeat;
                        NewNote.ChosenThrower = ThrowerIndex;

                        OutBeatMap.Add(NewNote);
                    }
                }
            }
        }
}