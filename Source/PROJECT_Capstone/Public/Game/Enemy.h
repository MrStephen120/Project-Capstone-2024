// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECT_CAPSTONE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	USkeletalMeshComponent* Mesh;
	UCapsuleComponent* MeshCollision;
	USphereComponent* PlayerDetection;
	AAIController* EnemyAIController;
	
public:
	// Sets default values for this character's properties
	AEnemy();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Handle Player Detection Logic
	UFUNCTION()
	virtual void OnPlayerDetected(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);
	//Handle Player Lost Logic
	UFUNCTION()
	virtual void OnPlayerLost(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
	//Enemy Behaviors
	UFUNCTION()
	void HandleChase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	UBehaviorTree* EnemyChaseBehaviorTree = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	bool isChasing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	float ChaseSpeed = 500.0f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
