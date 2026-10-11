// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingBall/RhythmStoreSubsystem.h"
#include "Ball.generated.h"

class UNiagaraComponent;
UENUM(BlueprintType)
enum class EHitQuality : uint8
{
    Max     UMETA(DisplayName = "Max"),     // Rainbow 300!
    Perfect UMETA(DisplayName = "Perfect"), // Normal 300
    Great   UMETA(DisplayName = "Great"),   // 200
    Good    UMETA(DisplayName = "Good"),    // 100
    Normal  UMETA(DisplayName = "Normal"),  // 50
    Miss    UMETA(DisplayName = "Miss")     // 0
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallHitSignature, EHitQuality, HitQuality);
UCLASS()
class PINGBALL_API ABall : public AActor
{
    GENERATED_BODY()
    
public: 
    // Sets default values for this actor's properties
    ABall();
    
    URhythmStoreSubsystem* RhythmStore;
    
    // The mesh component for the ball
    // (Note: Removed the stray semicolon you had at the end of the UPROPERTY macro here)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ballMesh;
    
    // Lerp variables for movement
    FVector StartLocation;
    FVector TargetLocation;

    float SpawnBeat;
    float TargetBeat;
    
    // Stores which color this ball randomly chose
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Color")
    int32 BallColorIndex;

    // Dynamic Material for the Ball
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
    UMaterialInstanceDynamic* DynamicMaterial;
    
    // ----------------------------------------------------
    // --- NEW: Impact Feel Variables for Smooth Scaling ---
    // ----------------------------------------------------
    FVector DefaultScale;
    FVector TargetScale;

    // How small the ball gets on a perfect hit
    UPROPERTY(EditAnywhere, Category = "Impact Feel")
    FVector ImpactSquishScale;

    // How fast the ball scales (higher number = faster/snappier)
    UPROPERTY(EditAnywhere, Category = "Impact Feel")
    float ScaleInterpSpeed;
    // ----------------------------------------------------
    //Niagara effect
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* HitEffectComponent;
    
    // 2. Add the Delegate event to the Ball
    UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
    FOnBallHitSignature OnBallHit;
    
    bool bWasHit = false;


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public: 
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    

    UFUNCTION(BlueprintCallable, Category = "Impact Feel")
    void TriggerPerfectHitSquish(EHitQuality HitType);   
    
    void InitializeBall(const FVector& InStartLocation, const FVector& InTargetLocation, const float& InSpawnBeat, const float& InTargetBeat)
    {
       this->StartLocation = InStartLocation;
       this->TargetLocation = InTargetLocation;
       this->SpawnBeat = InSpawnBeat;
       this->TargetBeat = InTargetBeat;
   
       SetActorLocation(StartLocation);

       // --- Color Randomization ---
   
       // Create the dynamic material so we can change its color
       DynamicMaterial = ballMesh->CreateAndSetMaterialInstanceDynamic(0);

       // Pick a random number between 1 and 3
       BallColorIndex = FMath::RandRange(1, 3);
       FLinearColor ChosenColor;

       // Assign the exact colors you use for the paddle
       if (BallColorIndex == 1) ChosenColor = FColor(229, 72, 103, 1).ReinterpretAsLinear();      // Red
       else if (BallColorIndex == 2) ChosenColor = FColor(183, 246, 105, 1).ReinterpretAsLinear(); // Green
       else ChosenColor = FColor(84, 158, 250, 1).ReinterpretAsLinear();                           // Blue

       if (DynamicMaterial)
       {
          DynamicMaterial->SetVectorParameterValue("Color", ChosenColor);
       }
    }

};