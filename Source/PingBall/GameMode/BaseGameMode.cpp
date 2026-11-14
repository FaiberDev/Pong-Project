#include "BaseGameMode.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Utilities/RhythmChartParser.h"
#include "PingBall/Ball/BallGenerator.h"
#include "PingBall/MusicPlayer/AMusicPlayer.h"
#include "PingBall/Data/RhythmSongDataAsset.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ABaseGameMode::ABaseGameMode()
{
	TotalScore = 0;
	InternalFloatScore = 0.0f;
	CurrentCombo = 0;
	MaxCombo = 0;
	TotalNotes = 0;
	BonusMultiplier = 1.0f;
	CurrentAccuracy = 100.0f;
	ActualAccuracyPoints = 0;
	MaxPossibleAccuracyPoints = 0;
	TargetMaxScore = 1000000.0f;
	FallbackChartFileName = TEXT("kamisama.json");
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ActiveSongAsset)
	{
		LoadAndStartSong(ActiveSongAsset);
		return;
	}

	// Fallback chart loading if no Song Data Asset is explicitly assigned
	TArray<FBeatMapNote> ParsedNotes;
	float ChartBPM = 120.0f;
	int32 ChartTimeSignature = 4;

	if (URhythmChartParser::ParseChart(FallbackChartFileName, ParsedNotes, ChartBPM, ChartTimeSignature))
	{
		URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
		if (Store)
		{
			Store->SetBPM(ChartBPM);
			Store->SetTimeSignature(ChartTimeSignature);
		}

		ABallGenerator* Generator = Cast<ABallGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ABallGenerator::StaticClass()));
		if (Generator)
		{
			Generator->BeatMap = ParsedNotes;
			TotalNotes = ParsedNotes.Num();
			UE_LOG(LogTemp, Log, TEXT("[BaseGameMode] Initialized with %d notes from fallback chart %s"),
				TotalNotes, *FallbackChartFileName);
		}
	}
}

void ABaseGameMode::LoadAndStartSong(URhythmSongDataAsset* SongAsset)
{
	if (!SongAsset)
	{
		return;
	}

	ActiveSongAsset = SongAsset;

	TArray<FBeatMapNote> ParsedNotes;
	float ChartBPM = SongAsset->DefaultBPM;
	int32 ChartTimeSignature = SongAsset->DefaultTimeSignature;

	// Parse using the song asset's customized MIDI pitch to lane mapping
	bool bSuccess = URhythmChartParser::ParseChartWithMapping(
		SongAsset->ChartJsonPath,
		SongAsset->MidiPitchToLaneMap,
		ParsedNotes,
		ChartBPM,
		ChartTimeSignature
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("[BaseGameMode] Failed to parse chart for song: %s"), *SongAsset->SongTitle.ToString());
		return;
	}

	// 1. Configure the central Rhythm Store
	URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
	if (Store)
	{
		Store->SetBPM(ChartBPM);
		Store->SetTimeSignature(ChartTimeSignature);
	}

	// 2. Supply notes to the Ball Generator
	ABallGenerator* Generator = Cast<ABallGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ABallGenerator::StaticClass()));
	if (Generator)
	{
		Generator->ResetSpawner();
		Generator->BeatMap = ParsedNotes;
	}

	// 3. Reset game mode match statistics
	TotalNotes = ParsedNotes.Num();
	TotalScore = 0;
	InternalFloatScore = 0.0f;
	CurrentCombo = 0;
	MaxCombo = 0;
	BonusMultiplier = 1.0f;
	CurrentAccuracy = 100.0f;
	ActualAccuracyPoints = 0;
	MaxPossibleAccuracyPoints = 0;

	// 4. Configure audio playback via Music Player
	AAMusicPlayer* MusicPlayer = Cast<AAMusicPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AAMusicPlayer::StaticClass()));
	if (MusicPlayer)
	{
		MusicPlayer->PlaySongAsset(SongAsset);
	}

	OnScoreUpdated.Broadcast(TotalScore, CurrentCombo, CurrentAccuracy);
	UE_LOG(LogTemp, Log, TEXT("[BaseGameMode] Successfully started song: %s (%d notes)"),
		*SongAsset->SongTitle.ToString(), TotalNotes);
}

void ABaseGameMode::RegisterHit(EHitQuality HitQuality)
{
	if (TotalNotes == 0) return;

	float AccuracyWeight = 0.0f;
	bool bComboBroken = false;

	switch (HitQuality)
	{
	case EHitQuality::Max:
		AccuracyWeight = 1.0f;
		BonusMultiplier = 1.0f;
		CurrentCombo++;
		ActualAccuracyPoints += 300;
		break;

	case EHitQuality::Perfect:
		AccuracyWeight = 1.0f;
		BonusMultiplier -= 0.05f;
		CurrentCombo++;
		ActualAccuracyPoints += 300;
		break;

	case EHitQuality::Great:
		AccuracyWeight = 0.66f;
		BonusMultiplier -= 0.20f;
		CurrentCombo++;
		ActualAccuracyPoints += 200;
		break;

	case EHitQuality::Good:
		AccuracyWeight = 0.33f;
		BonusMultiplier -= 0.40f;
		CurrentCombo++;
		ActualAccuracyPoints += 100;
		break;

	case EHitQuality::Normal:
		AccuracyWeight = 0.16f;
		BonusMultiplier -= 0.60f;
		CurrentCombo++;
		ActualAccuracyPoints += 50;
		break;

	case EHitQuality::Miss:
		AccuracyWeight = 0.0f;
		bComboBroken = true;
		break;
	}

	MaxPossibleAccuracyPoints += 300;

	// Combo & multiplier calculation
	if (bComboBroken)
	{
		CurrentCombo = 0;
		BonusMultiplier = 0.0f;
	}
	else
	{
		if (CurrentCombo > MaxCombo)
		{
			MaxCombo = CurrentCombo;
		}
		BonusMultiplier = FMath::Clamp(BonusMultiplier, 0.0f, 1.0f);
	}

	// 1-Million score distribution math (50% Base accuracy, 50% Combo bonus)
	const float MaxScorePerNote = TargetMaxScore / static_cast<float>(TotalNotes);
	const float BaseScoreGained = (MaxScorePerNote * 0.5f) * AccuracyWeight;
	const float BonusScoreGained = (MaxScorePerNote * 0.5f) * BonusMultiplier;

	InternalFloatScore += (BaseScoreGained + BonusScoreGained);
	TotalScore = FMath::RoundToInt32(InternalFloatScore);

	if (MaxPossibleAccuracyPoints > 0)
	{
		CurrentAccuracy = (static_cast<float>(ActualAccuracyPoints) / static_cast<float>(MaxPossibleAccuracyPoints)) * 100.0f;
	}

	if (GEngine)
	{
		const FString DebugMessage = FString::Printf(TEXT("Score: %d | Combo: %d | Acc: %.2f%%"), TotalScore, CurrentCombo, CurrentAccuracy);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DebugMessage);
	}

	OnScoreUpdated.Broadcast(TotalScore, CurrentCombo, CurrentAccuracy);
}