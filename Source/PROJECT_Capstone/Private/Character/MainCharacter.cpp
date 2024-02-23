// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Character/MainCharacterMovement.h"
#include "Game/MainCharacterController.h"

#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(MainCharacter);
// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& object)
{
	// AMainCharacter::AMainCharacter(const FObjectInitializer& object) : Super(object.SetDefaultSubobjectClass<UMainCharacterMovement>(ACharacter::CharacterMovementComponentName))
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorTickInterval(0.5f);
	SetActorTickEnabled(true);

	SetUpCharacterMovementSettings();
	SetUpCamera();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	HandleWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::UpdateMovementState()
{
	switch (CurrentState)
	{
	case EMovementState::Idle:
		break;
	case EMovementState::Walking:
		break;
	case EMovementState::Running:
		break;
	case EMovementState::Jumping:
		break;
	case EMovementState::DoubleJumping:
		break;
	case EMovementState::Diving:
		break;
	case EMovementState::LedgeGrabbing:
		break;
	case EMovementState::WallSliding:
		break;
	case EMovementState::WallJumping:
		break;
	}
}

void AMainCharacter::HandleWalkParticles()
{
	FVector ActorBounds = GetRootComponent()->Bounds.BoxExtent;
	float HalfHeight = ActorBounds.Z;

	if (!WalkingParticlesComponent)
	{
		WalkingParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
		(WalkSmokeTrail,
			GetRootComponent(),
			NAME_None,
			FVector(0.0f, 0.0f, -HalfHeight),
			FRotator(0.0f),
			EAttachLocation::SnapToTarget,
			true);
		//UE_LOG(LogTemp, Warning, TEXT("Walk Particle System not found, creating one."));

		WalkingParticlesComponent->SetNiagaraVariableFloat(TEXT("SpawnRate"), 0.0f);
		CanSpawnWalkParticles = false;
	}

	if (GetCharacterMovement()->IsMovingOnGround() && GetVelocity().Size() > 0)
	{
		if (WalkingParticlesComponent && !CanSpawnWalkParticles)
		{
				//UE_LOG(LogTemp, Warning, TEXT("Activating Particle System"));
				WalkingParticlesComponent->SetNiagaraVariableFloat(TEXT("SpawnRate"), 10.0f);
				CanSpawnWalkParticles = true;
		}
	}
	else
	{
		if (WalkingParticlesComponent && CanSpawnWalkParticles)
		{
			//UE_LOG(LogTemp, Warning, TEXT("De-Activating Particle System"));
			WalkingParticlesComponent->SetNiagaraVariableFloat(TEXT("SpawnRate"), 0.0f);
			CanSpawnWalkParticles = false;
		}
	}
}

void AMainCharacter::HandleJumpSmokeRing()
{
	FVector ActorBounds = GetRootComponent()->Bounds.BoxExtent;
	float HalfHeight = ActorBounds.Z;

	SmokeRingParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
	(JumpSmokeRing,
		GetRootComponent(),
		NAME_None,
		FVector(0.0f, 0.0f, -HalfHeight),
		FRotator(0.0f),
		EAttachLocation::SnapToTarget,
		true);
	UE_LOG(LogTemp, Warning, TEXT("Spawning Smoke Ring."));

	if (SmokeRingParticleComponent->IsComplete()) {
		SmokeRingParticleComponent->DestroyComponent();
	}
}

void AMainCharacter::SetUpCharacterMovementSettings()
{
	//Don't rotate character when the controller rotates. ONLY let it affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Configure character movement	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 5.0f;
	GetCharacterMovement()->GravityScale = 4.0f;
	GetCharacterMovement()->FallingLateralFriction = 0.1f;
	JumpMaxHoldTime = 0.1f;
	JumpMaxCount = 2.0f;
}

void AMainCharacter::SetUpCamera()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AMainCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void AMainCharacter::AirJump()
{	
	//Reset Jump Velocity
	GetCharacterMovement()->Velocity = FVector(GetVelocity().X, GetVelocity().Y, 0.0f);

	//JumpForce Velocity
	FVector JumpForceVel = (GetVelocity() + FVector(0.0f, 0.0f, 900.0f));
	LaunchCharacter(JumpForceVel, true, true);

	HandleJumpSmokeRing();
}

void AMainCharacter::Jump()
{
	Super::Jump();

	HandleJumpSmokeRing();
}

void AMainCharacter::StopJumping()
{
	Super::StopJumping();

}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	AMainCharacterController* MyController = Cast<AMainCharacterController>(GetController());
	if (MyController) {
		MyController->ResetJump();
	}

	HandleWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::Debug()
{
	// Get the current position of the character
	FVector CurrentPosition = GetActorLocation();

	// Draw a line from the previous position to the current position
	DrawDebugLine(
		GetWorld() ,
		PreviousPosition,  // Start point
		CurrentPosition,  // End point
		FColor::Red,  // Line color
		true,  // Persistent lines
		5.0f,  // Lifetime (negative means frame only)
		0,  // Depth priority
		2.0f  // Thickness
	);

	// Update the previous position for the next frame
	PreviousPosition = CurrentPosition;
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleWalkParticles();
	//Debug();
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
