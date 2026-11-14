#include "RhythmStoreSubsystem.h"

void URhythmStoreSubsystem::SetBPM(float NewBPM)
{
	CurrentBPM = NewBPM;
	OnBPMSet.Broadcast(NewBPM);
}

void URhythmStoreSubsystem::SetTimeSignature(int32 InTimeSignature)
{
	TimeSignature = InTimeSignature;
	OnBeatsPerBarSet.Broadcast(TimeSignature);
}

void URhythmStoreSubsystem::SetCurrentBeat(float InBeat)
{
	CurrentBeat = InBeat;
}

float URhythmStoreSubsystem::GetCurrentBeat() const
{
	if (FMath::IsNearlyZero(AudioOffsetMs) || FMath::IsNearlyZero(CurrentBPM))
	{
		return CurrentBeat;
	}

	// Convert offset (ms -> seconds -> beats)
	// Beats = Seconds * (BPM / 60)
	const float OffsetSeconds = AudioOffsetMs / 1000.0f;
	const float BeatsPerSecond = CurrentBPM / 60.0f;
	const float OffsetInBeats = OffsetSeconds * BeatsPerSecond;

	return CurrentBeat - OffsetInBeats;
}