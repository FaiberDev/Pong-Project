#include "RhythmSongDataAsset.h"
#include "Sound/SoundWave.h"

URhythmSongDataAsset::URhythmSongDataAsset()
{
	SongTitle = FText::FromString(TEXT("Kamisama"));
	DefaultBPM = 146.0f;
	DefaultTimeSignature = 4;
	AudioOffsetSeconds = 0.0f;
	ChartJsonPath = TEXT("kamisama.json");

	// Default sensible MIDI note mappings (E2, F2, G2)
	MidiPitchToLaneMap.Add(40, 0); // E2 -> Lane 0
	MidiPitchToLaneMap.Add(41, 1); // F2 -> Lane 1
	MidiPitchToLaneMap.Add(43, 2); // G2 -> Lane 2
}

void URhythmSongDataAsset::PostInitProperties()
{
	Super::PostInitProperties();

	// Ensure at least default mappings exist if empty
	if (MidiPitchToLaneMap.Num() == 0)
	{
		MidiPitchToLaneMap.Add(40, 0);
		MidiPitchToLaneMap.Add(41, 1);
		MidiPitchToLaneMap.Add(43, 2);
	}
}

int32 URhythmSongDataAsset::GetLaneForMidiPitch(int32 MidiPitch) const
{
	if (const int32* FoundLane = MidiPitchToLaneMap.Find(MidiPitch))
	{
		return *FoundLane;
	}

	// Fallback to Lane 0 with a non-fatal warning
	return 0;
}
