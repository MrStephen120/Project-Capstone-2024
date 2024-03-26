// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Enemy.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	StompCollision = FindComponentByClass<UBoxComponent>();
	checkf(MeshCollision, TEXT("Enemy: BoxComponent could not be initialized..."));
	
	PlayerDetection = FindComponentByClass<USphereComponent>();
	checkf(PlayerDetection, TEXT("Enemy: SphereComponent could not be initialized..."));
	
	EnemyAIController = Cast<AAIController>(GetController());
	checkf(EnemyAIController, TEXT("Enemy: Couldn't find AIController."));

	//Start Location is where it is placed.
	StartLocation = GetActorLocation();
	
	//Create the Events for the Collider
	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnPlayerDetected);
	PlayerDetection->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnPlayerLost);

	//Stomp Events
	StompCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnStompBegin);
}

void AEnemy::OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		// Player has been detected
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, "Player detected.");

		isChasing = true;
		
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"),OtherActor);
		HandleChaseStart();
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
		
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"),nullptr);
		HandleChaseStop();
	}
}

void AEnemy::HandleChaseStart()
{
	if (isChasing && !Dead)
	{
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		EnemyAIController->RunBehaviorTree(EnemyChaseBehaviorTree);
	}
}

void AEnemy::HandleChaseStop()
{
	if (!isChasing && !Dead)
	{
		GetCharacterMovement()->MaxWalkSpeed = WanderSpeed;
		EnemyAIController->RunBehaviorTree(EnemyWanderBehaviorTree);
	}
}

void AEnemy::OnStompBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		if (Player->GetCharacterMovement()->IsFalling())
		{
			Player->LaunchCharacter(FVector(0.0f,0.0f, StompLaunchForce), true, true);
			SquishEnemy();
		}
	}
	
}

void AEnemy::SquishEnemy()
{
	Dead = true;
	GetCharacterMovement()->StopMovementImmediately();
	EnemyAIController->BrainComponent->StopLogic("Stomped.");
	GetMesh()->SetWorldScale3D(FVector(1.5f, 1.5f, 0.25f));
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::DestroyEnemy, 2.0f, false);
}

void AEnemy::DestroyEnemy()
{
	this->Destroy();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


