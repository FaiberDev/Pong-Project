// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "DrawDebugHelpers.h"

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
	
	// 5th param (true) makes it permanent. 
	// 7th param (1) sets the Depth Priority so it renders ON TOP of the floor!
	DrawDebugLine(
		GetWorld(), 
		FVector(0.0, -2000.0, 60.0), 
		FVector(0.0, 2000.0, 60.0), 
		FColor::Red, 
		true,   // Persistent Line
		-1.0f,  // Infinite Lifetime
		1,      // Depth Priority (Draws on top of everything)
		10.0f   // Thickness
	);
	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!RhythmStore) return;
    
	float CurrentBeat = RhythmStore->GetCurrentBeat();
    
	// 1. Calculate the raw timing
	float RawAlpha = (CurrentBeat - SpawnBeat) / (TargetBeat - SpawnBeat);
    
	// 2. The Floor: Force it to never drop below 0.0 (prevents backward spawning)
	// Since there is no ceiling parameter here, it is free to naturally grow past 1.0!
	float Alpha = FMath::Max(0.0f, RawAlpha);
    
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
	SetActorLocation(NewLocation);
    
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Alpha: %f | CurrentBeat: %f"), Alpha, CurrentBeat));
	}

	// 3. Destroy once it sails safely past the paddle line
	if (Alpha >= 1.2f)
	{
		Destroy();
	}
}
