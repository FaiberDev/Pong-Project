#include "./BaseGameMode.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "PingBall/Utilities/RhythmChartParser.h"
#include "PingBall/Ball/BallGenerator.h" // Add this include!
#include "Kismet/GameplayStatics.h"      // Add this to search the level!

void ABaseGameMode::BeginPlay()
{   
	TArray<FBeatMapNote> ParsedNotes;
	float ChartBPM = 120.0f; 
	int32 ChartTimeSignature = 4; 
    
	// 1. The Parser does the heavy lifting
	if (URhythmChartParser::ParseChart("kamisama.json", ParsedNotes, ChartBPM, ChartTimeSignature))
	{
		URhythmStoreSubsystem* Store = GetWorld()->GetSubsystem<URhythmStoreSubsystem>();
		if (Store)
		{
			Store->SetBPM(ChartBPM); 
			Store->SetTimeSignature(ChartTimeSignature);
		}

		// 2. THE HAND-OFF: Find the BallGenerator in the level
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABallGenerator::StaticClass());
		ABallGenerator* Generator = Cast<ABallGenerator>(FoundActor);
       
		if (Generator)
		{
			// Hand the parsed notes over!
			Generator->BeatMap = ParsedNotes;
			UE_LOG(LogTemp, Warning, TEXT("Success! Handed %d notes to the Ball Generator."), Generator->BeatMap.Num());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find a BallGenerator in the level!"));
		}
	}else
	{
		// ADD THIS: So you know instantly if the file failed to load!
		UE_LOG(LogTemp, Error, TEXT("FATAL: Failed to parse kamisama.json! Check spelling and ensure the Data folder exists."));
	}
    
	Super::BeginPlay();
}