// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECT_CAPSTONE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	USkeletalMeshComponent* Mesh;
	UCapsuleComponent* MeshCollision;
	UBoxComponent* StompCollision;
	UBoxComponent* AttackCollision;
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
	void HandleChaseStart();

	UFUNCTION()
	void HandleChaseStop();

	//Chasing State Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	UBehaviorTree* EnemyChaseBehaviorTree = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	bool isChasing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase State")
	float ChaseSpeed = 400.0f;

	//Wandering State Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander State")
	UBehaviorTree* EnemyWanderBehaviorTree = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander State")
	bool isWandering = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander State")
	float WanderSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander State")
	FVector StartLocation = FVector(0.0f,0.0f,0.0f);

	//Stomp
	bool Dead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stomp")
	float StompLaunchForce = 750.0f;

	UFUNCTION()
	void OnStompBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void SquishEnemy();
	void DestroyEnemy();

	//Attack Player
	bool Attacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int Damage = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float KnockBackStrength = 1000.0f;
	UFUNCTION()
	void OnPlayerCollision(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void ResetAttack() { Attacking = false; };

	//Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* AttackSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SquishAudio;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
