// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "Ball.generated.h"

UCLASS()
class PINGBALL_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();
	
	
	URhythmStoreSubsystem* RhythmStore;
	
	// The mesh component for the ball
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	UStaticMeshComponent* ballMesh;
	
	
	// The projectile movement component for the ball
	
	
	//Lerp variables for movement
	FVector StartLocation;
	FVector TargetLocation;

	float SpawnBeat;
	float TargetBeat;
	
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	void InitializeBall(const FVector& InStartLocation, const FVector& InTargetLocation, const float& InSpawnBeat, const float& InTargetBeat)
	{
		this->StartLocation = InStartLocation;
		this->TargetLocation = InTargetLocation;
		this->SpawnBeat = InSpawnBeat;
		this->TargetBeat = InTargetBeat;
		
		SetActorLocation(StartLocation);
	}
	

};
