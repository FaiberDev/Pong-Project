#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "PingBall/RhythmTypes.h"
#include "ARythmPaddle.generated.h"

class UInputAction;
class UInputMappingContext;
class ABaseGameMode;
class URhythmStoreSubsystem;
class UMaterialInstanceDynamic;

/**
 * Player-controlled paddle pawn capable of horizontal tracking and quantized rhythmic strikes.
 */
UCLASS()
class PINGBALL_API AARythmPaddle : public APawn
{
	GENERATED_BODY()

public:
	AARythmPaddle();

	// Cached reference to the active GameMode
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rhythm|GameMode")
	TObjectPtr<ABaseGameMode> GameMode;

	// Cached reference to the rhythm store subsystem
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rhythm|Subsystem")
	TObjectPtr<URhythmStoreSubsystem> RhythmStore;

	// Paddle visual mesh component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> paddleMesh;

	// Movement speed when navigating horizontally
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float paddleSpeed = 300.0f;

	// Overlap detection volume for note hits
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> StrikeZone;

	// Enhanced Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Color1_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Color2_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Color3_Action;

	// Enhanced Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Dynamic material instance for color feedback
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	// Color definitions (kept for Blueprint backwards compatibility)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color0 = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // Default Neutral

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color1 = FLinearColor(0.898f, 0.282f, 0.404f, 1.0f); // Red

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color2 = FLinearColor(0.718f, 0.965f, 0.412f, 1.0f); // Green

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	FLinearColor Color3 = FLinearColor(0.329f, 0.620f, 0.980f, 1.0f); // Blue

	// Cooldown duration to prevent button mashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|AntiMash")
	float HitCooldownDuration = 0.05f;

	// Anti-mash lock state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rhythm|AntiMash")
	bool bCanHit = true;

	FTimerHandle HitCooldownTimer;

	// Event for Blueprint visual/audio feedback
	UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
	void TriggerHitGlow(EHitQuality HitQuality);

protected:
	virtual void BeginPlay() override;

	void ResetHitCooldown();

	// Tracks current active color index (0 = White, 1 = Red, 2 = Green, 3 = Blue)
	UPROPERTY(BlueprintReadOnly, Category = "Color")
	int32 CurrentColorIndex = 0;

	// Enhanced Input callbacks
	void OnMoveInput(const FInputActionValue& Value);
	void OnColor1Input(const FInputActionValue& Value);
	void OnColor2Input(const FInputActionValue& Value);
	void OnColor3Input(const FInputActionValue& Value);

	// Unified handler for color button strikes
	void HandleColorStrike(int32 ColorIndex);

	// Timing accuracy threshold windows (in beats)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Windows")
	float MaxWindow = 0.045f;     // Rainbow 300

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Windows")
	float PerfectWindow = 0.090f; // Normal 300

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Windows")
	float GreatWindow = 0.130f;   // 200

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Windows")
	float GoodWindow = 0.180f;    // 100

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm|Windows")
	float NormalWindow = 0.230f;  // 50

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetInputMappingContext();
	void MovePaddleHorizontal(float AxisValue, float DeltaTime);
	void CreateAndSetMaterialInstanceDynamic(int32 ElementIndex);
	void ChangePaddleColor(FLinearColor NewColor);

	// Evaluates overlapping balls in strike zone and scores hit accuracy
	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void AttemptHit();
};
