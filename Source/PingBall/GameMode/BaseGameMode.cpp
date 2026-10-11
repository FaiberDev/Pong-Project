#include "BaseGameMode.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Utilities/RhythmChartParser.h"
#include "PingBall/Ball/BallGenerator.h" 
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"      

/// 1. Update the Constructor to start at 100% Accuracy
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
}

void ABaseGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<FBeatMapNote> ParsedNotes;
    float ChartBPM = 120.0f; 
    int32 ChartTimeSignature = 4; 

    // 1. Parse the JSON file to fill the ParsedNotes array
    if (URhythmChartParser::ParseChart("kamisama.json", ParsedNotes, ChartBPM, ChartTimeSignature))
    {
        URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
        if (Store)
        {
            Store->SetBPM(ChartBPM); 
            Store->SetTimeSignature(ChartTimeSignature);
        }

        // 2. Search the level for the Ball Generator
        AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABallGenerator::StaticClass());
        ABallGenerator* Generator = Cast<ABallGenerator>(FoundActor);
       
        // 3. NOW the variable exists, so we can safely check it!
        if (Generator)
        {
            Generator->BeatMap = ParsedNotes;
            TotalNotes = ParsedNotes.Num();
            
            UE_LOG(LogTemp, Warning, TEXT("Success! Handed %d notes to the Ball Generator."), TotalNotes);
        }
    }
}

// 2. Update the RegisterHit function
void ABaseGameMode::RegisterHit(EHitQuality HitQuality)
{
    // Safety check: Don't do math if the level is empty!
    if (TotalNotes == 0) return; 

    float AccuracyWeight = 0.0f; 
    bool bComboBroken = false;

   
    switch (HitQuality)
    {
        case EHitQuality::Max: // Rainbow 300
            AccuracyWeight = 1.0f;     
            BonusMultiplier = 1.0f;    
            CurrentCombo++;
            ActualAccuracyPoints += 300;
            break;
            
        case EHitQuality::Perfect: // Normal 300
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
            
        case EHitQuality::Good: // 100
            AccuracyWeight = 0.33f;    
            BonusMultiplier -= 0.40f; 
            CurrentCombo++;
            ActualAccuracyPoints += 100;
            break;
            
        case EHitQuality::Normal: // 50
            AccuracyWeight = 0.16f;    
            BonusMultiplier -= 0.60f; 
            CurrentCombo++;
            ActualAccuracyPoints += 50;
            break;
            
        case EHitQuality::Miss: // 0
            AccuracyWeight = 0.0f;
            bComboBroken = true;
            break;
    }

    // The maximum possible accuracy points increases by 300 every single note
    MaxPossibleAccuracyPoints += 300; 

    // 2. MANAGE THE COMBO & MULTIPLIER
    if (bComboBroken)
    {
        CurrentCombo = 0;
        BonusMultiplier = 0.0f; // Drop bonus on a miss
    }
    else
    {
        if (CurrentCombo > MaxCombo) MaxCombo = CurrentCombo;
        // Clamp the multiplier so it never goes below 0 or above 1
        BonusMultiplier = FMath::Clamp(BonusMultiplier, 0.0f, 1.0f); 
    }

    // 3. THE OSU! 1-MILLION MATH
    float MaxScorePerNote = 1000000.0f / (float)TotalNotes;
    
    // Half goes to base accuracy, half goes to your combo bonus
    float BaseScoreGained = (MaxScorePerNote * 0.5f) * AccuracyWeight;
    float BonusScoreGained = (MaxScorePerNote * 0.5f) * BonusMultiplier;

    // Add it to the decimal score, then round it into the clean UI integer
    InternalFloatScore += (BaseScoreGained + BonusScoreGained);
    TotalScore = FMath::RoundToInt32(InternalFloatScore);

    // Calculate final Accuracy Percentage
    CurrentAccuracy = ((float)ActualAccuracyPoints / (float)MaxPossibleAccuracyPoints) * 100.0f;

    if (GEngine)
    {
        FString DebugMessage = FString::Printf(TEXT("Score: %d | Combo: %d | Acc: %.2f%%"), TotalScore, CurrentCombo, CurrentAccuracy);
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DebugMessage);
    }

    OnScoreUpdated.Broadcast(TotalScore, CurrentCombo, CurrentAccuracy);
}