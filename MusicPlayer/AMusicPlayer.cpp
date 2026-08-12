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
    
    InitializeClock();
    StartClockAndAudio();
}

void AAMusicPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAMusicPlayer::InitializeClock()
{
    

    FName ClockName = TEXT("MusicPlayer");
    FQuartzClockSettings ClockSettings;
    
    URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();

    if (Store)
    {
        Store->OnBeatsPerBarSet.AddDynamic(this, &AAMusicPlayer::OnBeatsPerBarChanged);
        ClockSettings.TimeSignature.NumBeats = Store->BeatsPerBar;
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
    URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
    
    if (Store)
    {
        Store->OnBPMSet.AddDynamic(this, &AAMusicPlayer::OnBPMChanged);
        QuartzClockHandle->SetBeatsPerMinute(this, FQuartzQuantizationBoundary(), FOnQuartzCommandEventBP(), QuartzClockHandle, Store->GetBPM());
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
    UE_LOG(LogTemp, Warning, TEXT("Beat: %d, Bar: %d"), Beat, NumBars);

    URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
    if (Store)
    {
       Store->OnBeat.Broadcast(Beat); 
    }
    // Log the current BPM
    if (QuartzClockHandle)
    {
        UE_LOG(LogTemp, Warning, TEXT("BPM: %f"), QuartzClockHandle->GetBeatsPerMinute(this));
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