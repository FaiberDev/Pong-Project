// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallGenerator.generated.h"

UCLASS()
class PINGBALL_API ABallGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallGenerator();
	
	int NumberOfTowers = 5;
	//Asked to the Store
	int32 BPM = 0;
	
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Throwers")
	TArray<USceneComponent*> Throwers;

	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void LookAheadForNextBeat(int32 CurrentBeatNumber);
	
	// UFUNCTION()
	//void GeatBeatFromStore(int32 BeatNumber);
	
	
};
