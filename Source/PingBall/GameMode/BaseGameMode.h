#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PingBall/Ball/Ball.h" 
#include "BaseGameMode.generated.h"

// 1. UPGRADE TO THREE PARAMS: Now includes NewAccuracy!
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnScoreUpdated, int32, NewScore, int32, NewCombo, float, NewAccuracy);

UCLASS()
class PINGBALL_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
    
public:
	ABaseGameMode(); 

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Scoring")
	FOnScoreUpdated OnScoreUpdated;

	// --- SCORE VARIABLES ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 TotalNotes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 TotalScore;
	float InternalFloatScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 CurrentCombo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 MaxCombo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	float BonusMultiplier;

	// --- NEW: ACCURACY VARIABLES ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	float CurrentAccuracy; // The percentage (e.g., 98.5%)

	// Internal trackers (hidden from Blueprints)
	int32 ActualAccuracyPoints; 
	int32 MaxPossibleAccuracyPoints; 

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void RegisterHit(EHitQuality HitQuality);
	
	



};