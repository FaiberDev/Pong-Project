#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "AMusicPlayer.generated.h"

class UQuartzClockHandle;
class UAudioComponent;
class USoundWave;
class URhythmSongDataAsset;

/**
 * Manages low-latency audio playback and beat synchronization via Unreal's Quartz Subsystem.
 */
UCLASS()
class PINGBALL_API AAMusicPlayer : public AActor
{
	GENERATED_BODY()

public:
	AAMusicPlayer();

	UPROPERTY(Transient)
	TObjectPtr<URhythmStoreSubsystem> RhythmStore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quartz")
	UQuartzClockHandle* QuartzClockHandle = nullptr;

	// Audio Component to handle playback
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playback")
	TObjectPtr<UAudioComponent> AudioComponent;

	// Sound Wave to be played
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playback")
	TObjectPtr<USoundWave> SoundWave;

	// Current playback time in milliseconds
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Playback")
	float TrackTimeMs = 0.0f;

	// Optional song data asset to drive audio and timing configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playback")
	TObjectPtr<URhythmSongDataAsset> ActiveSongData;

	// Name identifier for this Quartz clock instance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quartz")
	FName ClockName = TEXT("MusicPlayerClock");

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnQuartzBeat(FName InClockName, EQuartzCommandQuantization QuantizationType, int32 NumBars, int32 Beat, float FractionalBeat);

	UFUNCTION()
	void OnBPMChanged(float NewBPM);

	UFUNCTION()
	void OnBeatsPerBarChanged(int32 NewBeatsPerBar);

	UFUNCTION(BlueprintCallable, Category = "Quartz")
	void InitializeClock();

	UFUNCTION(BlueprintCallable, Category = "Quartz")
	void StartClockAndAudio();

	// Sets a new song asset and resets the audio clock
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void PlaySongAsset(URhythmSongDataAsset* SongAsset);
};
