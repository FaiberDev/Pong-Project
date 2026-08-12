// Fill out your copyright notice in the Description page of Project Settings.


#include "ARythmPaddle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

// Sets default values
AARythmPaddle::AARythmPaddle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Create the paddle mesh component and set it as the root component
	paddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleMesh"));
	RootComponent = paddleMesh;
	paddleSpeed = 400.0f; // Default speed for paddle movement
	
	Color0 = FColor(255, 255, 255, 1).ReinterpretAsLinear(); // White
	Color1 = FColor(229, 72, 103, 1).ReinterpretAsLinear(); // Red
	Color2 = FColor(183, 246,105, 1).ReinterpretAsLinear(); //Green
	Color3 = FColor(84,158,250, 1).ReinterpretAsLinear(); //Blue
	
	
	

}

// Called when the game starts or when spawned
void AARythmPaddle::BeginPlay()
{
	Super::BeginPlay();
	//Set input context 
	SetInputMappingContext();
	CreateAndSetMaterialInstanceDynamic(0);
	ChangePaddleColor(Color0);

	
	
}
// Called every frame
void AARythmPaddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Called to bind functionality to input
void AARythmPaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast the default component to UE5's UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind the action to our movement function
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARythmPaddle::OnMoveInput);
		EnhancedInputComp->BindAction(Color1_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor1Input);
		EnhancedInputComp->BindAction(Color2_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor2Input);
		EnhancedInputComp->BindAction(Color3_Action, ETriggerEvent::Started, this, &AARythmPaddle::OnColor3Input);
	}
}

void AARythmPaddle::MovePaddleHorizontal(float AxisValue, float DeltaTime)
{
	if (AxisValue != 0.0f)
	{
		FVector Movement = FVector(0.0f, AxisValue * paddleSpeed * DeltaTime, 0.0f);
		AddActorLocalOffset(Movement, true);
		
	}
}
void AARythmPaddle::OnMoveInput(const FInputActionValue& Value)
{
	// Extract the scalar float value from the input action
	float AxisValue = Value.Get<float>();

	// Pass the input value and the frame delta time into your movement logic
	MovePaddleHorizontal(AxisValue, GetWorld()->GetDeltaSeconds());
}

void AARythmPaddle::SetInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add the input mapping context
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


// Change the paddle color based on the input value
void AARythmPaddle::CreateAndSetMaterialInstanceDynamic(int32 ElementIndex)
{
	DynamicMaterial = paddleMesh->CreateAndSetMaterialInstanceDynamic(ElementIndex);
}

// Change the paddle color based on the input value
void AARythmPaddle::ChangePaddleColor(FLinearColor NewColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Color", NewColor);
	}
}

void AARythmPaddle::OnColor1Input(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Color 1 Input Triggered"));
	ChangePaddleColor(Color1);
}
void AARythmPaddle::OnColor2Input(const FInputActionValue& Value)
{
	ChangePaddleColor(Color2);
}
void AARythmPaddle::OnColor3Input(const FInputActionValue& Value)
{
	ChangePaddleColor(Color3);
}

