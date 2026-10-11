// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "../GameMode/BaseGameMode.h"
#include "ARythmPaddle.generated.h"



class UInputAction;
class UInputMappingContext;

UCLASS()
class PINGBALL_API AARythmPaddle : public APawn
{
	GENERATED_BODY()

public:
	
	//Base GameMode
	ABaseGameMode* GameMode;
	
	// Sets default values for this pawn's properties
	AARythmPaddle();
	
	// Paddle Mesh Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UStaticMeshComponent* paddleMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	float paddleSpeed; // Speed at which the paddle moves
	
	// Collision Box for Hit Detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* StrikeZone;
	
	//Pointer to store  Input Action Asset from the Editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction*  Color1_Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Color2_Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Color3_Action;
	
	
	
	//Mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	
	//Dynamic Material Instance for the paddle

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UMaterialInstanceDynamic* DynamicMaterial;
	
	//Color Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor
	Color1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color3;
	
	// Anti-Mash lock
	bool bCanHit = true;
	
	// The timer that controls the cooldown
	FTimerHandle HitCooldownTimer;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
	void TriggerHitGlow(EHitQuality HitQuality);	
protected:
	
	
	//function that unlocks the paddle
	void ResetHitCooldown();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Tracks the current color (0 = White, 1 = Red, 2 = Green, 3 = Blue)
	UPROPERTY(BlueprintReadOnly, Category = "Color")
	int32 CurrentColorIndex = 0;
	
	
	/// Input Action Functions
	void OnMoveInput(const FInputActionValue& Value);
	void OnColor1Input(const FInputActionValue& Value);
	void OnColor2Input(const FInputActionValue& Value);
	void OnColor3Input(const FInputActionValue& Value);
	
	// --- TIMING WINDOWS ---
	UPROPERTY(EditAnywhere, Category = "Rhythm")
	float MaxWindow = 0.045f;     // Rainbow 300 

	UPROPERTY(EditAnywhere, Category = "Rhythm")
	float PerfectWindow = 0.090f; // Normal 300

	UPROPERTY(EditAnywhere, Category = "Rhythm")
	float GreatWindow = 0.130f;   // 200

	UPROPERTY(EditAnywhere, Category = "Rhythm")
	float GoodWindow = 0.180f;    // 100

	UPROPERTY(EditAnywhere, Category = "Rhythm")
	float NormalWindow = 0.230f;  // 50
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//Set Input Mapping Context
	void SetInputMappingContext();
	
	//Player Movement -  Control with the mouse 
	void MovePaddleHorizontal(float AxisValue, float DeltaTime);
	
	//Create and set dynamic material instance for the paddle
	void CreateAndSetMaterialInstanceDynamic(int32 ElementIndex);
	
	//change the color of the paddle
	void ChangePaddleColor(FLinearColor NewColor);
	
	// Checks for overlapping balls and calculates rhythm accuracy
	void AttemptHit();
	
 	 

};
