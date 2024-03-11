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
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"

DEFINE_LOG_CATEGORY(MainCharacter);
// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& object)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetUpCharacterMovementSettings();
	SetUpCamera();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize the timelines
	InitializeSquashStretchTimelines();
	
	//Initialize Particles
	HandleWalkParticles();
	HandleJumpSmokeRing();
	HandleDiveParticles();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateState();
	//DebugState();
	Debug();
	
	//Handle Timeline ticks
	TimelineTicks(DeltaTime);
	//Grounded Check
	HandleGrounded();
	//Idle Check
	if (IsCharacterIdle())
	{
		ChangeState(EMovementState::Idle);
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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, DefaultYRotationRate, 0.0f);
	GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity;
	GetCharacterMovement()->AirControl = DefaultAirControl;
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
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
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

void AMainCharacter::DiveSqueezeUpdate(float alpha)
{
	GetMesh()->SetRelativeScale3D(FMath::Lerp(BaseScale, BaseScale * DiveSqueezeFactor, alpha ));
}

void AMainCharacter::DiveSqueezeFinish()
{
}
void AMainCharacter::JumpSqueezeUpdate(float alpha)
{
	GetMesh()->SetRelativeScale3D(FMath::Lerp(BaseScale, BaseScale * JumpSqueezeFactor, alpha ));
}

void AMainCharacter::JumpSqueezeFinish()
{
}

void AMainCharacter::LandSquishUpdate(float alpha)
{
	GetMesh()->SetRelativeScale3D(FMath::Lerp(BaseScale, BaseScale * LandSquishFactor, alpha ));
}

void AMainCharacter::LandSquishFinish()
{
}

void AMainCharacter::InitializeSquashStretchTimelines()
{
	//Initialize Base Scale
	BaseScale = GetMesh()->GetRelativeScale3D();

	//DiveSqueeze Timeline Initialization
	FOnTimelineFloat FDiveSqueezeUpdate;
	FOnTimelineEvent FDiveSqueezeFinished;
	FDiveSqueezeUpdate.BindUFunction(this, FName("DiveSqueezeUpdate"));
	FDiveSqueezeFinished.BindUFunction(this, FName("DiveSqueezeFinish"));
	DiveSqueezeTimeline.AddInterpFloat(SquashStretchCurve, FDiveSqueezeUpdate);
	DiveSqueezeTimeline.SetLooping(false);
	DiveSqueezeTimeline.SetTimelineLength(DiveLength);
	DiveSqueezeTimeline.SetTimelineFinishedFunc(FDiveSqueezeFinished);
	
	//JumpSqueeze Timeline Initialization.
	FOnTimelineFloat FJumpSqueezeUpdate;
	FOnTimelineEvent FJumpSqueezeFinished;
	FJumpSqueezeUpdate.BindUFunction(this, FName("JumpSqueezeUpdate"));
	FJumpSqueezeFinished.BindUFunction(this, FName("JumpSqueezeFinish"));
	JumpSqueezeTimeline.AddInterpFloat(SquashStretchCurve, FJumpSqueezeUpdate);
	JumpSqueezeTimeline.SetLooping(false);
	JumpSqueezeTimeline.SetTimelineLength(0.25f);
	JumpSqueezeTimeline.SetTimelineFinishedFunc(FJumpSqueezeFinished);
	
	//LandSquash Timeline Initialization.
	FOnTimelineFloat FSquishProgressUpdate;
	FOnTimelineEvent FSquishFinished;
	FSquishFinished.BindUFunction(this, FName("LandSquishFinish"));
	FSquishProgressUpdate.BindUFunction(this, FName("LandSquishUpdate"));
	LandSquishTimeline.AddInterpFloat(SquashStretchCurve, FSquishProgressUpdate);
	LandSquishTimeline.SetLooping(false);
	LandSquishTimeline.SetTimelineLength(0.25f);
	LandSquishTimeline.SetTimelineFinishedFunc(FSquishFinished);
}

void AMainCharacter::TimelineTicks(float deltaTime)
{
	JumpSqueezeTimeline.TickTimeline(deltaTime);
	LandSquishTimeline.TickTimeline(deltaTime);
	DiveSqueezeTimeline.TickTimeline(deltaTime);
}

void AMainCharacter::ChangeState(EMovementState NewState)
{
	CurrentState = NewState;
	//DebugState();
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
		//HandleWallSlideState();
		break;
	case EMovementState::WallJump:
		HandleWallJumpState();
		break;
	}
}

void AMainCharacter::HandleGrounded()
{
	if (IsCharacterMovingOnGround())
	{
		ChangeState(EMovementState::Running);
	}
	else if (!IsCharacterMovingOnGround())
	{
		ChangeState(EMovementState::InAir);
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
	if (CanJump)
	{
		if (!IsWallSliding && !CanWallJump && JumpCount < JumpMaxCount) //Normal Jumps
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
		else if (CanWallJump) //Wall Jump
		{
			ChangeState(EMovementState::WallJump);
		}
	}
}

void AMainCharacter::HandleJumpState()
{
	//Jump
	Jump();
	++JumpCount;

	//Play Timeline
	JumpSqueezeTimeline.PlayFromStart();
	
	//Handle Particles
	DeActivateWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::HandleInAirState()
{
	//Check if in-air and is by a wall.
	if (CanWallSlide())
	{
		//Start Wall Sliding if true. Called by CanWallSlide().
		ChangeState(EMovementState::WallSlide);
	}
	else
	{
		//State is isInAir
		//Gravity back to Normal if been modified.
		if (!isDiving)
		{
			IsWallSliding = false;
			WallSlideStart = false;
			
			GetCharacterMovement()->GravityScale = DefaultGravity;
		}
	}
	
	DeActivateWalkParticles();
}

void AMainCharacter::HandleAirJumpState()
{
	//AirJump
	AirJump();
	++JumpCount;
	
	//Handle Particles
	HandleJumpSmokeRing();
	//Play Timeline
	JumpSqueezeTimeline.PlayFromStart();
}

void AMainCharacter::HandleLandingState()
{
	//Redundant Method, All Landing changes are put inside Landed Event Function.
}

void AMainCharacter::HandleDivingState()
{
	if (CanDive)
	{
		ResetToDefaults();
		Dive();
		
		//VFX Here
		ActivateDiveParticles();
		//Play Timeline
		DiveSqueezeTimeline.PlayFromStart();
	}
}

void AMainCharacter::HandleWallSlideState(FRotator outHitNormal)
{
	//Called Inside CanWallSlide() method.
	if (bCanWallSlide)
	{
		if (GetVelocity().Z <= 0.0f)
		{
			if (!WallSlideStart)
			{
				//Do Once
				WallSlideStart = true;
				IsWallSliding = true;
				CanWallJump = true;
				CanJump = true;
				
				CanDive = false;//Disable Diving while WallSliding
				isDiving = false;
				
				//Immediately Stop Character
				GetCharacterMovement()->Velocity = FVector(GetVelocity().X, GetVelocity().Y, 0.0f);
				LandSquishTimeline.PlayFromStart();
			}
			
			GetCharacterMovement()->GravityScale = WallSlideGravityScale;
			GetCharacterMovement()->Velocity = FMath::VInterpTo(
				GetVelocity(),
				FVector(0.0f,0.0f,GetVelocity().Z),
				GetWorld()->DeltaTimeSeconds,
				WallSlideDeceleration);
		
			SetActorRotation(FRotator(0.0f,outHitNormal.Yaw, 0.0f));
		}
	}
}

void AMainCharacter::HandleWallJumpState()
{
	CanWallJump = false;
	CanJump = false;
	CanDive = true;
	//Do WallJump
	//Set CharacterMovement Values
	GetCharacterMovement()->GravityScale = DefaultGravity;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,0.0f,0.0f);
	GetCharacterMovement()->AirControl = 0.0f;
	
	//WallJump Math
	FVector WallJumpForce = ((GetActorForwardVector() * WallJumpSpeed) + GetActorUpVector() * DefaultJumpZVelocity);
	GetCharacterMovement()->Launch(WallJumpForce);

	//Stretch
	JumpSqueezeTimeline.PlayFromStart();
}

bool AMainCharacter::IsCharacterIdle()
{
	//Check if Velocity is close to ZERO
	const FVector Velocity = GetVelocity();
	const bool IsMoving = !Velocity.IsNearlyZero((1.0f));
	//Check if there are no WASD inputs.
	return !IsMoving;
}

bool AMainCharacter::IsCharacterMovingOnGround()
{
	if (GetCharacterMovement()->IsWalking())
	{
		FVector Velocity = GetVelocity();
		float Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
		return Speed > KINDA_SMALL_NUMBER;
	}
	return false;
}

bool AMainCharacter::CanWallSlide()
{
	if (bCanWallSlide && GetCharacterMovement()->IsFalling())
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("CapsuleTrace")), true);
		QueryParams.AddIgnoredActor(this);
		
		bool bHitOccurred = GetWorld()->SweepSingleByChannel(
			HitResult,
			GetActorLocation(), //Start
			GetActorLocation(), //End
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeCapsule(25.0f, 35.0f),
			QueryParams
			);

		if (bHitOccurred)
		{
			// Hit
			if (HitResult.bBlockingHit)
			{
				HandleWallSlideState(FRotator(HitResult.Normal.Rotation()));
				return HitResult.bBlockingHit;
			}
		}
		else
		{
			return false;
		}
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

void AMainCharacter::ActivateDiveParticles()
{
	//Activate
	if (!CanDiveParticles && DiveParticleComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Activating Particle System"));
		DiveParticleComponent->SetNiagaraVariableFloat(TEXT("RateSpawn"), 15.0f);
		CanDiveParticles = true;
	}
}

void AMainCharacter::DeActivateDiveParticles()
{
	//Deactivate
	if (CanDiveParticles && DiveParticleComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Activating Particle System"));
		DiveParticleComponent->SetNiagaraVariableFloat(TEXT("RateSpawn"), 0.0f);
		CanDiveParticles = false;
	}
}

void AMainCharacter::HandleDiveParticles()
{
	FVector ActorBounds = GetRootComponent()->Bounds.BoxExtent;
	float HalfHeight = ActorBounds.Z;
	if (!DiveParticleComponent)
	{
		DiveParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
		(DiveTrail,
		GetRootComponent(),
		NAME_None,
		FVector(0.0f, 0.0f, HalfHeight),
		FRotator(0.0f),
		EAttachLocation::SnapToTarget,
		true);
		UE_LOG(LogTemp, Warning, TEXT("Spawning Dive Particles."));
		DiveParticleComponent->SetNiagaraVariableFloat(TEXT("RateSpawn"), 0.0f);
		CanDiveParticles = false;
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
	//Set Bools
	isDiving = true;
	CanDive = false;
	CanJump = false;
	
	//Delay Timer
	FTimerHandle DiveTimer;
	//Set CharacterMovement Values
	GetCharacterMovement()->GravityScale = DiveGravityScale;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,0.0f,0.0f);
	GetCharacterMovement()->AirControl = 0.0f;
	
	//Dive Math
	FVector DiveForce = ((GetActorForwardVector() * DiveSpeed) + GetActorUpVector() * 300.0f);
	GetCharacterMovement()->Launch(DiveForce);
	
	//Start Cooldown Timer
	GetWorld()->GetTimerManager().SetTimer(DiveTimer, this, &AMainCharacter::DiveEnd, DiveLength, false);
}
void AMainCharacter::DiveEnd()
{
	isDiving = false;
	ResetToDefaults();
	DeActivateDiveParticles();
}
void AMainCharacter::ResetDive()
{
	isDiving = false;
	CanDive = true;
	DeActivateDiveParticles();
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
	//Maybe increase gravity over 0.3 seconds?
}

void AMainCharacter::ResetToDefaults()
{
	// Reset gravity scale
	GetCharacterMovement()->GravityScale = DefaultGravity;
	//Reset Rotation Rate
	GetCharacterMovement()->RotationRate = FRotator(0.0f,DefaultYRotationRate,0.0f);
	//Reset Air Control
	GetCharacterMovement()->AirControl = DefaultAirControl;
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	ChangeState(EMovementState::Landing);
	
	IsWallSliding = false;
	WallSlideStart = false;
	CanWallJump = false;
	
	ResetJump();
	ResetDive();
	ResetToDefaults();

	//VFX & SFX
	LandSquishTimeline.PlayFromStart();

	//Reset/DeActivate Particles
	HandleWalkParticles();
	HandleJumpSmokeRing();
}

void AMainCharacter::Debug()
{
	if (ShowDebugLines)
	{
		// Get the current position of the character
		CurrentPosition = GetActorLocation();

		// Draw a line from the previous position to the current position
		DrawDebugLine(
			GetWorld() ,
			PreviousPosition,  // Start point
			CurrentPosition,  // End point
			FColor::Red,  // Line color
			false,  // Persistent lines
			3.0f,  // Lifetime (negative means frame only)
			0,  // Depth priority
			2.0f  // Thickness
		);

		// Update the previous position for the next frame
		PreviousPosition = CurrentPosition;
	}
}

void AMainCharacter::DebugState()
{
	if (ShowDebugStates)
	{
		const FString DebugText = FString::Printf(TEXT("Current State: %s"), *UEnum::GetValueAsString(CurrentState));
		const FColor TextColor = FColor::Green;
		float Duration = 0.1f;
		GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, DebugText);	
	}
}

void AMainCharacter::DebugText(FString Text)
{
	const FColor TextColor = FColor::Red;
	float Duration = 3.0f;
	GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, Text);
}
