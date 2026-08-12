// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"


// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = ballMesh;

}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{

	Super::BeginPlay();

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();	
	

	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector TargetLoc = FVector(0.0f, 0.0f, 230.0f);
	float CurrentBeat = RhythmStore->GetCurrentBeat();
	float Alpha = (CurrentBeat - SpawnBeat) / (TargetBeat - SpawnBeat);
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLoc, Alpha);
	SetActorLocation(NewLocation);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Alpha: %f | CurrentBeat: %f"), Alpha, CurrentBeat));
	}

	if (Alpha >= 1.0f)
	{
		Destroy();
	}
	

}

