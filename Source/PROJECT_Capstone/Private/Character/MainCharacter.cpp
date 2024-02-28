// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Character/MainCharacterMovement.h"
#include "Game/MainCharacterController.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(MainCharacter);
// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& object)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.1f);
	SetActorTickEnabled(true);

	//Get Animator and Appropriate Values
	if (!AnimInstance)
    {
		AnimInstance = GetMesh()->GetAnimInstance();
	}
	if (AnimInstance)
	{
		bDiveAnimProperty = FindFProperty<FBoolProperty>(AnimInstance->GetClass(), "Dive");
	}
	
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

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateState();
	
	DebugState();
	Debug();

	if (IsCharacterIdle())
	{
		ChangeState(EMovementState::Idle);
	}
	else if (IsCharacterMovingOnGround())
	{
		ChangeState(EMovementState::Running);
	}
	else if (!IsCharacterMovingOnGround())
	{
		ChangeState(EMovementState::InAir);
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
	GetCharacterMovement()->JumpZVelocity = 750.0f;
	GetCharacterMovement()->AirControl = 5.0f;
	GetCharacterMovement()->GravityScale = DefaultGravity;
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

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainCharacter::ChangeState(EMovementState NewState)
{
	CurrentState = NewState;
}

void AMainCharacter::UpdateState()
{
	switch (CurrentState)
	{
	case EMovementState::Idle:
		HandleIdleState();
		break;
	case EMovementState::Walking:
		break;
	case EMovementState::Running:
		HandleRunningState();
		break;
	case EMovementState::Jumping:
		HandleJumpState();
		break;
	case EMovementState::InAir:
		HandleInAirState();
		break;
	case EMovementState::AirJump:
		HandleAirJumpState();
		break;
	case EMovementState::Landing:
		HandleLandingState();
		break;
	case EMovementState::Diving:
		HandleDivingState();
		break;
	case EMovementState::WallSlide:
		//HandleWallSideState(DeltaTime);
		break;
	case EMovementState::WallJump:
		break;
	}
}

void AMainCharacter::HandleIdleState()
{
	DeActivateWalkParticles();
}

void AMainCharacter::HandleRunningState()
{
	HandleWalkParticles();
}

void AMainCharacter::HandleJumpRequest()
{
	if (CanJump && JumpCount < JumpMaxCount)
	{
		if (!GetCharacterMovement()->IsFalling())
		{ //Ground Jump
			ChangeState(EMovementState::Jumping);
		}
		//For Future Use: Check if it's a wall jump, if false then it's an air jump.
		else
		{ //AirJump
			ChangeState(EMovementState::AirJump);
		}
	}
}

void AMainCharacter::HandleJumpState()
{
	//Jump
	Jump();
	++JumpCount;

	//Handle Particles
	DeActivateWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::HandleInAirState()
{
	DeActivateWalkParticles();
}

void AMainCharacter::HandleAirJumpState()
{
	//AirJump
	AirJump();
	++JumpCount;

	//Handle Particles
	HandleJumpSmokeRing();
}

void AMainCharacter::HandleLandingState()
{
	
}

void AMainCharacter::HandleDivingState()
{
	if (bDiveAnimProperty != nullptr)
	{
		DebugText("Wee I'm Diving!");
		bDiveAnimProperty->SetPropertyValue_InContainer(AnimInstance, true);
	}
	
	Dive();
}

bool AMainCharacter::IsCharacterIdle()
{
	//Check if Velocity is close to ZERO
	const FVector Velocity = GetVelocity();
	const bool IsMoving = !Velocity.IsNearlyZero((1.0f));
	//Check if there are no WASD inputs.
	return !IsMoving;
}

bool AMainCharacter::IsCharacterGrounded()
{
	return GetCharacterMovement()->IsWalking();
}

bool AMainCharacter::IsCharacterMovingOnGround()
{
	if (IsCharacterGrounded())
	{
		FVector Velocity = GetVelocity();
		float Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
		return Speed > KINDA_SMALL_NUMBER;
	}
	return false;
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
	
	if (IsCharacterMovingOnGround())
	{
		ActivateWalkParticles();
	}
	else
	{
		DeActivateWalkParticles();
	}
}

void AMainCharacter::ActivateWalkParticles()
{
	if (WalkingParticlesComponent && !CanSpawnWalkParticles)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Activating Particle System"));
		WalkingParticlesComponent->SetNiagaraVariableFloat(TEXT("SpawnRate"), 10.0f);
		CanSpawnWalkParticles = true;
	}
}

void AMainCharacter::DeActivateWalkParticles()
{
	if (WalkingParticlesComponent && CanSpawnWalkParticles)
	{
		//UE_LOG(LogTemp, Warning, TEXT("De-Activating Particle System"));
		WalkingParticlesComponent->SetNiagaraVariableFloat(TEXT("SpawnRate"), 0.0f);
		CanSpawnWalkParticles = false;
	}
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
	FVector JumpForceVel = (GetVelocity() + FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity));
	GetCharacterMovement()->Launch(JumpForceVel);
}

void AMainCharacter::Dive()
{
	//Dive Math
}

void AMainCharacter::Jump()
{
	//Reset Jump Velocity
	GetCharacterMovement()->Velocity = FVector(GetVelocity().X, GetVelocity().Y, 0.0f);

	//JumpForce Velocity
	FVector JumpForceVel = (GetVelocity() + FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity));
	GetCharacterMovement()->Launch(JumpForceVel);
}

void AMainCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	ChangeState(EMovementState::Landing);
	AMainCharacterController* MyController = Cast<AMainCharacterController>(GetController());
	if (MyController) {
		MyController->ResetJump();
	}
	// Reset gravity scale
	GetCharacterMovement()->GravityScale = DefaultGravity;
	
	ResetJump();
	HandleWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::Debug()
{
	// Get the current position of the character
	CurrentPosition = GetActorLocation();

	// Draw a line from the previous position to the current position
	DrawDebugLine(
		GetWorld() ,
		PreviousPosition,  // Start point
		CurrentPosition,  // End point
		FColor::Red,  // Line color
		true,  // Persistent lines
		1.0f,  // Lifetime (negative means frame only)
		0,  // Depth priority
		2.0f  // Thickness
	);

	// Update the previous position for the next frame
	PreviousPosition = CurrentPosition;
}

void AMainCharacter::DebugState()
{
	const FString DebugText = FString::Printf(TEXT("Current State: %s"), *UEnum::GetValueAsString(CurrentState));
	const FColor TextColor = FColor::Green;
	float Duration = 0.1f;
	GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, DebugText);
}

void AMainCharacter::DebugText(FString Text)
{
	const FString DebugText = Text;
	const FColor TextColor = FColor::Red;
	float Duration = 3.0f;
	GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, DebugText);
}
