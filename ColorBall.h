// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorBall.generated.h"

UCLASS()
class PINGBALL_API AColorBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorBall();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	UStaticMeshComponent* ballMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Properties")
	float startingVelocity;
	

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnBallHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()	
	void MultiplyVelocity(float  multiplier);
	
	
	void SetBallColor(FString newColor);
	
	void getCurrentMaterial();
	
private:
	class UMaterialInstanceDynamic* DynamicMaterial;
};
