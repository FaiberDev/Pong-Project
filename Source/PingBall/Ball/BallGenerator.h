// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallGenerator.generated.h"

USTRUCT(BlueprintType)
struct FBeatMapNote
{
	GENERATED_BODY()
	
	// The beat number at which the note should be spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatMap")
	int32 BeatToSpawn = 0;
	
	// The index of the thrower that should spawn the note
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatMap")
	int32  ChosenThrower = 0;
	
};


UCLASS()
class PINGBALL_API ABallGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallGenerator();
	
	//Asked to the Store
	int32 BPM = 0;
	
	// The beat map hold the struct that defines when notes should be spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatMap")
	TArray<FBeatMapNote> BeatMap;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwers", meta = (MakeEditWidget = true))
	TArray<FTransform> Throwers;

	int32 CurrentAbsoluteBeat = -3;
	
	int32 LookAheadOffSet = 4; // How many beats to look ahead for the next beat
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ball")
	TSubclassOf<AActor>  BallClass;
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void GetBeatFromStore(int32 BeatNumber);
	
	void LookAheadForNextBeat(int32 TargetBeatNumber);
	
	
};
