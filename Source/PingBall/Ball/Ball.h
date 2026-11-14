#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingBall/RhythmTypes.h"
#include "Ball.generated.h"

class UNiagaraComponent;
class URhythmStoreSubsystem;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallHitSignature, EHitQuality, HitQuality);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallRecycleSignature, ABall*, RecycledBall);

/**
 * Rhythm target ball that travels from a lane thrower towards the paddle strike zone.
 * Supports both standard lifecycle and high-performance object pooling.
 */
UCLASS()
class PINGBALL_API ABall : public AActor
{
	GENERATED_BODY()

public:
	ABall();

	UPROPERTY(Transient)
	TObjectPtr<URhythmStoreSubsystem> RhythmStore;

	// Mesh component for the ball
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ballMesh;

	// Niagara hit particle effect component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> HitEffectComponent;

	// Dynamic material instance for color tinting
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	// Movement lerp properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Movement")
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Movement")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Timing")
	float SpawnBeat = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Timing")
	float TargetBeat = 0.0f;

	// Color index assigned to this ball (1 = Red, 2 = Green, 3 = Blue)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Color")
	int32 BallColorIndex = 1;

	// Squish animation settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact Feel")
	FVector ImpactSquishScale = FVector(0.2f, 0.2f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact Feel")
	float ScaleInterpSpeed = 20.0f;

	// Hit event broadcast
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
	FOnBallHitSignature OnBallHit;

	// Pool recycle event broadcast
	UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
	FOnBallRecycleSignature OnBallRecycle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|State")
	bool bWasHit = false;

	// Indicates whether this ball is currently in flight (used by Object Pool)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|Pool")
	bool bIsActiveInPool = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FVector DefaultScale = FVector::OneVector;

	UPROPERTY()
	FVector TargetScale = FVector::OneVector;

	FTimerHandle DespawnTimerHandle;

public:
	virtual void Tick(float DeltaTime) override;

	// Configures and launches the ball
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Setup")
	void InitializeBall(
		const FVector& InStartLocation,
		const FVector& InTargetLocation,
		float InSpawnBeat,
		float InTargetBeat,
		int32 InColorIndex = 0
	);

	// Squishes the ball and registers success
	UFUNCTION(BlueprintCallable, Category = "Impact Feel")
	void TriggerPerfectHitSquish(EHitQuality HitType);

	// Deactivates the ball and returns it to pool or destroys it
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Pool")
	void DeactivateBall();

	// Sets active state for pooling
	void SetPoolActive(bool bActive);
};