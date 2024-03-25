// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Enemy.h"
#include "Character/MainCharacter.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	//Initialization
	Mesh = FindComponentByClass<USkeletalMeshComponent>();
	checkf(Mesh, TEXT("Enemy: Mesh could not be initialized..."));
	MeshCollision = FindComponentByClass<UCapsuleComponent>();
	checkf(MeshCollision, TEXT("Enemy: CylinderComponent could not be initialized..."));
	PlayerDetection = FindComponentByClass<USphereComponent>();
	checkf(PlayerDetection, TEXT("Enemy: SphereComponent could not be initialized..."));
	EnemyAIController = Cast<AAIController>(GetController());
	checkf(EnemyAIController, TEXT("Enemy: Couldn't find AIController."));

	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnPlayerDetected);
	PlayerDetection->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnPlayerLost);
}

void AEnemy::OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		// Player has been detected
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, "Player detected.");

		isChasing = true;
	}
}

void AEnemy::OnPlayerLost(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		// Player has been lost
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Player lost.");
		
		isChasing = false;
	}
}

void AEnemy::HandleChase()
{
	if (isChasing)
	{
		EnemyAIController->RunBehaviorTree(EnemyChaseBehaviorTree);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


