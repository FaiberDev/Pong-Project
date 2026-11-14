#include "BallGenerator.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "Ball.h"
#include "Engine/World.h"

ABallGenerator::ABallGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	TargetHitLocation = FVector(0.0f, 0.0f, 60.0f);
	LookAheadOffSet = 4;
	bUseObjectPooling = true;
	InitialPoolSize = 25;
	CurrentAbsoluteBeat = 0;
	NextNoteIndex = 0;
}

void ABallGenerator::BeginPlay()
{
	Super::BeginPlay();

	RhythmStore = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	if (RhythmStore)
	{
		RhythmStore->OnBeat.AddDynamic(this, &ABallGenerator::GetBeatFromStore);
	}

	if (bUseObjectPooling)
	{
		InitializeObjectPool();
	}
}

void ABallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABallGenerator::InitializeObjectPool()
{
	if (!BallClass || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < InitialPoolSize; ++i)
	{
		ABall* PooledBall = GetWorld()->SpawnActor<ABall>(BallClass, FTransform::Identity, SpawnParams);
		if (PooledBall)
		{
			PooledBall->SetPoolActive(false);
			PooledBall->OnBallRecycle.AddDynamic(this, &ABallGenerator::OnBallRecycled);
			BallPool.Add(PooledBall);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BallGenerator] Object pool warmed up with %d ball actors."), BallPool.Num());
}

ABall* ABallGenerator::AcquireBallFromPool()
{
	// 1. Search for an available inactive ball in the pool
	for (ABall* Ball : BallPool)
	{
		if (IsValid(Ball) && !Ball->bIsActiveInPool)
		{
			return Ball;
		}
	}

	// 2. Pool exhausted: dynamically grow pool by spawning a new instance
	if (BallClass && GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABall* NewBall = GetWorld()->SpawnActor<ABall>(BallClass, FTransform::Identity, SpawnParams);
		if (NewBall)
		{
			NewBall->SetPoolActive(false);
			NewBall->OnBallRecycle.AddDynamic(this, &ABallGenerator::OnBallRecycled);
			BallPool.Add(NewBall);
			UE_LOG(LogTemp, Warning, TEXT("[BallGenerator] Pool capacity expanded. Total pool size: %d"), BallPool.Num());
			return NewBall;
		}
	}

	return nullptr;
}

void ABallGenerator::OnBallRecycled(ABall* RecycledBall)
{
	// Ball has already reset its visibility/collision; bookkeeping can be extended here
}

void ABallGenerator::GetBeatFromStore(int32 BeatNumber)
{
	CurrentAbsoluteBeat++;
	const int32 TargetBeatNumber = CurrentAbsoluteBeat + LookAheadOffSet;

	if (TargetBeatNumber >= 1)
	{
		LookAheadForNextBeat(TargetBeatNumber);
	}
}

void ABallGenerator::LookAheadForNextBeat(int32 TargetBeatNumber)
{
	// O(1) amortized streaming: iterate notes from the current cursor until notes surpass TargetBeatNumber
	while (BeatMap.IsValidIndex(NextNoteIndex) && BeatMap[NextNoteIndex].BeatToSpawn <= TargetBeatNumber)
	{
		if (BeatMap[NextNoteIndex].BeatToSpawn == TargetBeatNumber)
		{
			SpawnBallForNote(BeatMap[NextNoteIndex]);
		}
		NextNoteIndex++;
	}
}

void ABallGenerator::SpawnBallForNote(const FBeatMapNote& Note)
{
	if (!BallClass)
	{
		return;
	}

	// Safety check 1: bounds check lane/thrower index
	if (!Throwers.IsValidIndex(Note.ChosenThrower))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BallGenerator] Invalid lane index %d (Max available lanes: %d)"),
			Note.ChosenThrower, Throwers.Num());
		return;
	}

	// 1. Calculate world-space spawn and strike locations
	const FTransform ThrowerTransform = Throwers[Note.ChosenThrower] * GetActorTransform();
	const FVector StartLocation = ThrowerTransform.GetLocation();
	const FVector TargetLocation = FVector(TargetHitLocation.X, StartLocation.Y, TargetHitLocation.Z);

	// 2. Query exact audio beat from subsystem
	float ExactSpawnBeat = RhythmStore ? RhythmStore->GetCurrentBeat() : static_cast<float>(CurrentAbsoluteBeat);

	// 3. Acquire ball from pool or spawn actor
	ABall* BallInstance = nullptr;
	if (bUseObjectPooling)
	{
		BallInstance = AcquireBallFromPool();
	}
	else
	{
		BallInstance = GetWorld()->SpawnActor<ABall>(BallClass, ThrowerTransform);
	}

	if (BallInstance)
	{
		BallInstance->InitializeBall(
			StartLocation,
			TargetLocation,
			ExactSpawnBeat,
			ExactSpawnBeat + static_cast<float>(LookAheadOffSet),
			Note.NoteColorIndex
		);
	}
}

void ABallGenerator::ResetSpawner()
{
	CurrentAbsoluteBeat = 0;
	NextNoteIndex = 0;

	for (ABall* Ball : BallPool)
	{
		if (IsValid(Ball))
		{
			Ball->DeactivateBall();
		}
	}
}