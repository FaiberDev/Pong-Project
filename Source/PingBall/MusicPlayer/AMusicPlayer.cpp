#include "AMusicPlayer.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "../RhythmStoreSubsystem.h"

// Sets default values
AAMusicPlayer::AAMusicPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

// Called when the game starts or when spawned
void AAMusicPlayer::BeginPlay()
{
    Super::BeginPlay();
    RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
    InitializeClock();
    StartClockAndAudio();
}

void AAMusicPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    float CurrentBPM = RhythmStore->GetBPM();
    float BPS = CurrentBPM / 60.0f; // Beats per second
    float DeltaBeat = BPS * DeltaTime; // How many beats have passed in this frame
    float CurrentBeat = RhythmStore->GetCurrentBeat();
    CurrentBeat += DeltaBeat;
    RhythmStore->SetCurrentBeat(CurrentBeat);
}

void AAMusicPlayer::InitializeClock()
{
    

    FName ClockName = TEXT("MusicPlayer");
    FQuartzClockSettings ClockSettings;
    

    if (RhythmStore)
    {
        RhythmStore->OnBeatsPerBarSet.AddDynamic(this, &AAMusicPlayer::OnBeatsPerBarChanged);
        ClockSettings.TimeSignature.NumBeats = RhythmStore->BeatsPerBar;
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
    // Get BPM From Store and Start clock
    
    if (RhythmStore)
    {
        RhythmStore->OnBPMSet.AddDynamic(this, &AAMusicPlayer::OnBPMChanged);
        QuartzClockHandle->SetBeatsPerMinute(this, FQuartzQuantizationBoundary(), FOnQuartzCommandEventBP(), QuartzClockHandle, RhythmStore->GetBPM());
    }   
    QuartzClockHandle->StartClock(this, QuartzClockHandle);
    
    //  Bind and Subscribe to Beats
    FOnQuartzMetronomeEventBP BeatDelegate;
    BeatDelegate.BindDynamic(this, &AAMusicPlayer::OnQuartzBeat);   
    QuartzClockHandle->SubscribeToQuantizationEvent(this, EQuartzCommandQuantization::Beat, BeatDelegate,  QuartzClockHandle);
    
    // Play Audio Quantized
    FQuartzQuantizationBoundary Boundary;
    Boundary.Quantization = EQuartzCommandQuantization::None;
    
    if (SoundWave && AudioComponent)
    {
       AudioComponent->SetSound(SoundWave);
       AudioComponent->PlayQuantized(this, QuartzClockHandle, Boundary, FOnQuartzCommandEventBP());
    }
}

void AAMusicPlayer::OnQuartzBeat(FName ClockName, EQuartzCommandQuantization QuantizationType, int32 NumBars, int32 Beat, float FractionalBeat)
{
   // UE_LOG(LogTemp, Warning, TEXT("Beat: %d, Bar: %d"), Beat, NumBars);

    if (RhythmStore)
    {
       RhythmStore->OnBeat.Broadcast(Beat); 
    }
    // Log the current BPM
    if (QuartzClockHandle)
    {
       // UE_LOG(LogTemp, Warning, TEXT("BPM: %f"), QuartzClockHandle->GetBeatsPerMinute(this));
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
    if (QuartzClockHandle)
    {
        UE_LOG(LogTemp, Warning, TEXT("BeatsPerBar changed to: %d"), NewBeatsPerBar);
    }
}