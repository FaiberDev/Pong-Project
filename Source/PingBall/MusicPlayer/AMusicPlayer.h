// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "AMusicPlayer.generated.h"


class UQuartzClockHandle;
UCLASS()
class PINGBALL_API AAMusicPlayer : public AActor
{
	GENERATED_BODY()
	
	URhythmStoreSubsystem* RhythmStore;


public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quartz")
	UQuartzClockHandle* QuartzClockHandle;
	
	
	// Sets default values for this actor's properties
	AAMusicPlayer();
	
	// Audio Component to handle playback
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playback")
	UAudioComponent* AudioComponent;
	// Sound Wave to be played
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Playback")
	USoundWave* SoundWave;
	// Current playback time in milliseconds
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Playback")
	float trackTimeMs;
	

	
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnQuartzBeat(FName ClockName, EQuartzCommandQuantization QuantizationType, int32 NumBars, int32 Beat, float FractionalBeat);
	void OnBPMChanged(float NewBPM);
	
	// Function to handle changes in BeatsPerBar
	UFUNCTION()
	void OnBeatsPerBarChanged(int32 NewBeatsPerBar);
	
	void InitializeClock();
	void StartClockAndAudio();
};
