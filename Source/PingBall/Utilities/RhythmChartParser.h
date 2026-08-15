#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PingBall/Ball/BallGenerator.h"
#include "RhythmChartParser.generated.h"

UCLASS()
class PINGBALL_API URhythmChartParser : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
    
public:
	// Takes a filename and returns the parsed notes array and the BPM found in the file
	UFUNCTION(BlueprintCallable, Category = "Rhythm Data")
	static bool ParseChart(FString FileName, TArray<FBeatMapNote>& OutBeatMap, float& OutBPM, int32& OutTimeSignature);
    
	static void GiveChartToBallGenerator(TSharedPtr<FJsonObject> RootObject, TArray<FBeatMapNote>& OutBeatMap, float OutBPM);
};