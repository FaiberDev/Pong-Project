#include "AMusicPlayer.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Data/RhythmSongDataAsset.h"

AAMusicPlayer::AAMusicPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	ClockName = TEXT("MusicPlayerClock");
	TrackTimeMs = 0.0f;
}

void AAMusicPlayer::BeginPlay()
{
	Super::BeginPlay();

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();

	if (ActiveSongData)
	{
		PlaySongAsset(ActiveSongData);
	}
	else
	{
		InitializeClock();
		StartClockAndAudio();
	}
}

void AAMusicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!RhythmStore || !QuartzClockHandle) return;

	// 1. Get exact elapsed seconds from the Quartz audio clock
	const float RunningSeconds = QuartzClockHandle->GetEstimatedRunTime(this);
	TrackTimeMs = RunningSeconds * 1000.0f;

	// 2. Mathematically calculate continuous beat
	const float CurrentBPM = RhythmStore->GetBPM();
	if (CurrentBPM > 0.0f)
	{
		const float BPS = CurrentBPM / 60.0f;
		const float ExactCurrentBeat = RunningSeconds * BPS;

		// 3. Update subsystem single source of truth
		RhythmStore->SetCurrentBeat(ExactCurrentBeat);
	}
}

void AAMusicPlayer::InitializeClock()
{
	FQuartzClockSettings ClockSettings;

	if (RhythmStore)
	{
		RhythmStore->OnBeatsPerBarSet.AddDynamic(this, &AAMusicPlayer::OnBeatsPerBarChanged);
		ClockSettings.TimeSignature.NumBeats = RhythmStore->GetTimeSignature();
	}

	UQuartzSubsystem* QuartzSubsystem = UQuartzSubsystem::Get(GetWorld());
	if (QuartzSubsystem)
	{
		QuartzClockHandle = QuartzSubsystem->CreateNewClock(this, ClockName, ClockSettings);
	}
}

void AAMusicPlayer::StartClockAndAudio()
{
	if (!QuartzClockHandle) return;

	if (RhythmStore)
	{
		RhythmStore->OnBPMSet.AddDynamic(this, &AAMusicPlayer::OnBPMChanged);
		QuartzClockHandle->SetBeatsPerMinute(this, FQuartzQuantizationBoundary(), FOnQuartzCommandEventBP(), QuartzClockHandle, RhythmStore->GetBPM());
	}

	QuartzClockHandle->StartClock(this, QuartzClockHandle);

	// Subscribe to beat quantization events
	FOnQuartzMetronomeEventBP BeatDelegate;
	BeatDelegate.BindDynamic(this, &AAMusicPlayer::OnQuartzBeat);
	QuartzClockHandle->SubscribeToQuantizationEvent(this, EQuartzCommandQuantization::Beat, BeatDelegate, QuartzClockHandle);

	// Play audio quantized
	FQuartzQuantizationBoundary Boundary;
	Boundary.Quantization = EQuartzCommandQuantization::None;

	if (SoundWave && AudioComponent)
	{
		AudioComponent->SetSound(SoundWave);
		AudioComponent->PlayQuantized(this, QuartzClockHandle, Boundary, FOnQuartzCommandEventBP());
	}

	if (RhythmStore)
	{
		RhythmStore->OnSongStarted.Broadcast();
	}
}

void AAMusicPlayer::PlaySongAsset(URhythmSongDataAsset* SongAsset)
{
	if (!SongAsset) return;

	ActiveSongData = SongAsset;

	if (SongAsset->SongAudio)
	{
		SoundWave = SongAsset->SongAudio;
	}

	if (RhythmStore)
	{
		RhythmStore->SetBPM(SongAsset->DefaultBPM);
		RhythmStore->SetTimeSignature(SongAsset->DefaultTimeSignature);
	}

	InitializeClock();
	StartClockAndAudio();
}

void AAMusicPlayer::OnQuartzBeat(FName InClockName, EQuartzCommandQuantization QuantizationType, int32 NumBars, int32 Beat, float FractionalBeat)
{
	if (RhythmStore)
	{
		// Calculate the true global beat: (Bars * BeatsPerBar) + Beat
		const int32 TimeSig = RhythmStore->GetTimeSignature() > 0 ? RhythmStore->GetTimeSignature() : 4;
		const int32 AbsoluteBeat = (NumBars * TimeSig) + FMath::Max(0, Beat - 1);

		RhythmStore->OnBeat.Broadcast(AbsoluteBeat);
	}
}

void AAMusicPlayer::OnBPMChanged(float NewBPM)
{
	if (QuartzClockHandle)
	{
		QuartzClockHandle->SetBeatsPerMinute(this, FQuartzQuantizationBoundary(), FOnQuartzCommandEventBP(), QuartzClockHandle, NewBPM);
	}
}

void AAMusicPlayer::OnBeatsPerBarChanged(int32 NewBeatsPerBar)
{
	UE_LOG(LogTemp, Log, TEXT("[MusicPlayer] BeatsPerBar changed to: %d"), NewBeatsPerBar);
}