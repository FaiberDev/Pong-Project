#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingBall/RhythmTypes.h"
#include "BallGenerator.generated.h"

class ABall;
class URhythmStoreSubsystem;

/**
 * Spawns and manages rhythm target balls along defined thrower lanes.
 * Features chronological note streaming (O(1) amortized) and an actor object pool.
 */
UCLASS()
class PINGBALL_API ABallGenerator : public AActor
{
	GENERATED_BODY()

public:
	ABallGenerator();

	// Reference to the active rhythm store subsystem
	UPROPERTY(Transient)
	TObjectPtr<URhythmStoreSubsystem> RhythmStore;

	// Loaded beat map containing all notes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatMap")
	TArray<FBeatMapNote> BeatMap;

	// Transform widgets for each thrower/lane origin (local space)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwers", meta = (MakeEditWidget = true))
	TArray<FTransform> Throwers;

	// Target strike location (relative to this actor or world coordinates)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwers", meta = (MakeEditWidget = true))
	FVector TargetHitLocation = FVector(0.0f, 0.0f, 60.0f);

	// How many beats ahead of the strike zone notes should spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	int32 LookAheadOffSet = 4;

	// The Ball actor class to spawn/pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	TSubclassOf<ABall> BallClass;

	// Enables object pooling to eliminate runtime GC hitches
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
	bool bUseObjectPooling = true;

	// Number of balls to pre-allocate at BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
	int32 InitialPoolSize = 25;

	// Current beat counter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	int32 CurrentAbsoluteBeat = 0;

	// Index cursor into BeatMap for O(1) chronological note streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	int32 NextNoteIndex = 0;

protected:
	virtual void BeginPlay() override;

	// Pre-allocated actor pool for balls
	UPROPERTY(Transient)
	TArray<TObjectPtr<ABall>> BallPool;

	void InitializeObjectPool();
	ABall* AcquireBallFromPool();

	UFUNCTION()
	void OnBallRecycled(ABall* RecycledBall);

public:
	virtual void Tick(float DeltaTime) override;

	// Triggered on every quantized beat broadcast from Quartz
	UFUNCTION()
	void GetBeatFromStore(int32 BeatNumber);

	// Evaluates and streams notes arriving in the lookahead window
	void LookAheadForNextBeat(int32 TargetBeatNumber);

	// Spawns/activates a single note ball safely
	void SpawnBallForNote(const FBeatMapNote& Note);

	// Resets the spawner cursor and clears active balls (for restarts/new songs)
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Spawner")
	void ResetSpawner();
};
