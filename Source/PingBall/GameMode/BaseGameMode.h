#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PingBall/RhythmTypes.h"
#include "BaseGameMode.generated.h"

class URhythmSongDataAsset;
class ABallGenerator;
class AAMusicPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnScoreUpdated, int32, NewScore, int32, NewCombo, float, NewAccuracy);

/**
 * GameMode managing rhythm match scoring, combo multipliers, and song initialization.
 */
UCLASS()
class PINGBALL_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseGameMode();

	virtual void BeginPlay() override;

	// Active song asset driving audio, timing, and chart mapping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Song")
	TObjectPtr<URhythmSongDataAsset> ActiveSongAsset;

	// Fallback JSON filename in Content/Data/ if no Song Data Asset is specified
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Song")
	FString FallbackChartFileName = TEXT("kamisama.json");

	// Event broadcast whenever score, combo, or accuracy updates
	UPROPERTY(BlueprintAssignable, Category = "Scoring")
	FOnScoreUpdated OnScoreUpdated;

	// --- Scoring State ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 TotalNotes = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 TotalScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 CurrentCombo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 MaxCombo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	float BonusMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	float CurrentAccuracy = 100.0f;

	// Target maximum total score for a full combo run (standard rhythm game scale)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring|Tuning")
	float TargetMaxScore = 1000000.0f;

	// Internal trackers
	int32 ActualAccuracyPoints = 0;
	int32 MaxPossibleAccuracyPoints = 0;
	float InternalFloatScore = 0.0f;

	// Registers a note hit or miss and recalculates combo and accuracy
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void RegisterHit(EHitQuality HitQuality);

	// Loads and starts a song data asset dynamically
	UFUNCTION(BlueprintCallable, Category = "Rhythm|Song")
	void LoadAndStartSong(URhythmSongDataAsset* SongAsset);
};