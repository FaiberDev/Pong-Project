// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorBall.h"

#include "NaniteSceneProxy.h"

// Sets default values
AColorBall::AColorBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	
	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = ballMesh;
	
	startingVelocity = 100.0f;
	

	

}

// Called when the game starts or when spawned


void AColorBall::BeginPlay()

{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Ball BeginPlay"));
	
	// Ensure our mesh component exists before trying to push i
	if (ballMesh)
	{	
		
		//Restrict to 2D plane 
		ballMesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);
		
		//Enable Physics simulation
		ballMesh->SetSimulatePhysics(true);
		ballMesh->WakeRigidBody();
		
		// Define  velocity
		FVector LaunchDirection = FVector(1.0f, 0.0f, 0.0f);
		FVector MovementVelocity = LaunchDirection * startingVelocity;
		
		
		ballMesh->PutRigidBodyToSleep();
		ballMesh->WakeRigidBody();
			UMaterialInterface* BaseMaterial = ballMesh->GetMaterial(0);

		
		
		// Directly set the physics velocity to shoot the ball forward instantly
		ballMesh->SetPhysicsLinearVelocity(MovementVelocity);
		OnActorHit.AddDynamic(this, &AColorBall::OnBallHit);
		getCurrentMaterial();

	}

	
	

	
}

// Called every frame
void AColorBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AColorBall::OnBallHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	// Check if the other actor is valid and not the ball itself
	if (OtherActor && OtherActor != this)
	{
		// Log the hit event for debugging
		UE_LOG(LogTemp, Warning, TEXT("Ball hit something! Other Actor: %s"), *OtherActor->GetName());

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
		}
		MultiplyVelocity(10.0f); // Example multiplier, you can adjust as needed

	}
	float randomRed = FMath::FRandRange(0.0f, 1.0f);
	float randomGreen = FMath::FRandRange(0.0f, 1.0f);
	float randomBlue = FMath::FRandRange(0.0f, 1.0f);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(FName("Param"),	FLinearColor(randomRed,randomGreen,randomBlue,1)); //  color
	}
}
void AColorBall::MultiplyVelocity(float multiplier)

{
	
	if (ballMesh)
	{
		
		FVector CurrentVelocity = ballMesh->GetPhysicsLinearVelocity();
		
		if (CurrentVelocity.IsNearlyZero())
		{
			CurrentVelocity = FVector(100.0f, 0.0f, 0.0f); 
		}
		
		FVector NewVelocity = CurrentVelocity * multiplier;
		NewVelocity.Z = 0.0f;
			
		const float MaxVelocity = 10000.0f; // Define a maximum velocity limit
		UE_LOG(LogTemp, Warning, TEXT("New Velocity: %s"), *NewVelocity.ToString());
		
		if (NewVelocity.Size() > MaxVelocity)
		{	
			NewVelocity = NewVelocity.GetSafeNormal() * MaxVelocity;
		}
		

		ballMesh->SetPhysicsLinearVelocity(NewVelocity);
	}
	

}

void AColorBall::getCurrentMaterial()
{
	UMaterialInterface* BaseMaterial = ballMesh->GetMaterial(0);
	if (BaseMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (DynamicMaterial)
		{
			ballMesh->SetMaterial(0, DynamicMaterial);
		}
	}

}