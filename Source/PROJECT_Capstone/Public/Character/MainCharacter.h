// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Character.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/TimelineComponent.h"
#include "NiagaraSystem.h"
#include "MainCharacter.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(MainCharacter, Log, All);
class UCurveFloat;

USTRUCT(BlueprintType)
struct FMovementParameters
{
	GENERATED_BODY()

	// Walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Walking")
	float WalkSpeed;
	float MaxSpeed;

	// Running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Running")
	float RunSpeed;

	// Jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jumping")
	float JumpHeight;

	// Wall Sliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Diving")
	float DiveSpeed;
	
	// Wall Sliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall Sliding")
	float WallSlideSpeed;
};

UENUM()
enum class EMovementState
{
	Idle,
	Walking,
	Running,
	Jumping,
	InAir,
	AirJump,
	Landing,
	Diving,
	WallSlide,
	WallJump,
};

UCLASS()
class PROJECT_CAPSTONE_API AMainCharacter : public ACharacter
{
private:
	GENERATED_BODY()
	// Sets default values for this character's properties
	AMainCharacter(const FObjectInitializer& object);
	//Getters for Camera Components
private:
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	//Set up CharacterMovement Settings
	void SetUpCharacterMovementSettings();
	//Set up Camera Settings
	void SetUpCamera();
	//SetUp Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	//Particles
	//Dive Trail
	UPROPERTY(EditAnywhere, Category = "Particles")
	UNiagaraSystem* DiveTrail;
	//Dust Trail
	UPROPERTY(EditAnywhere, Category = "Particles")
	UNiagaraSystem* WalkSmokeTrail;
	//Jump Dust Ring
	UPROPERTY(EditAnywhere, Category = "Particles")
	UNiagaraSystem* JumpSmokeRing;

private:
	//* Particles
	//Dive Trail
	UNiagaraComponent* DiveParticleComponent;
	bool CanDiveParticles;
	//Dust Trail
	UNiagaraComponent* WalkingParticlesComponent;
	bool CanSpawnWalkParticles;
	//Jump Dust Ring
	UNiagaraComponent* SmokeRingParticleComponent;
	bool CanSmokeRingParticles;

	//Particle Methods
	//Dust Particles when walking.
	void HandleWalkParticles();
	void ActivateWalkParticles();
	void DeActivateWalkParticles();

	//Particles when jumping/landing
	void HandleJumpSmokeRing();

	//Particles when dashing/diving
	void ActivateDiveParticles();
	void DeActivateDiveParticles();
	void HandleDiveParticles();

//* Squash & Stretch Section *//
protected:
	//Squash & Stretch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squash & Stretch")
	FVector BaseScale;
	//Get Float Curve
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Squash & Stretch")
	UCurveFloat* SquashStretchCurve = nullptr;
	//DiveSqueeze Timeline;
	FTimeline DiveSqueezeTimeline;
	UFUNCTION()
	void DiveSqueezeUpdate(float alpha);
	UFUNCTION()
	void DiveSqueezeFinish();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squash & Stretch")
	FVector DiveSqueezeFactor = FVector(0.6f,1.6f,0.6f);
	//JumpSqueeze Timeline
	FTimeline JumpSqueezeTimeline;
	UFUNCTION()
	void JumpSqueezeUpdate(float alpha);
	UFUNCTION()
	void JumpSqueezeFinish();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squash & Stretch")
	FVector JumpSqueezeFactor = FVector(0.6f,0.6f,1.25f);
	//LandSquash Timeline
	FTimeline LandSquishTimeline;
	UFUNCTION()
	void LandSquishUpdate(float alpha);
	UFUNCTION()
	void LandSquishFinish();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squash & Stretch")
	FVector LandSquishFactor = FVector (1.25f,1.25f,0.6f);
	//Initialize Squash & Stretch Timelines
	void InitializeSquashStretchTimelines();
	void TimelineTicks(float deltaTime);
//* End of Squash & Stretch Section *//

public:
	//Changes State of Player Character
	void ChangeState(EMovementState NewState);
	//Current movement state of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement State")
	EMovementState CurrentState;
protected:
	//Updates State on Tick
	void UpdateState();
	//==|State Specific Methods|==
	//Grounded
	void HandleGrounded();
	//Idle
	void HandleIdleState();
	//Run
	void HandleRunningState();
	//Jump
public: void HandleJumpRequest();
protected: void HandleJumpState();
	//In-Air
	void HandleInAirState();
	//AirJump
	void HandleAirJumpState();
	//Landing
	void HandleLandingState();
	//Diving
	void HandleDivingState();
	//WallSliding
	void HandleWallSlideState(FRotator outHitNormal);
	//WallJump
	void HandleWallJumpState();
	
	//Check if Character has no inputs and velocity = 0;
	bool IsCharacterIdle();
	//Check if Character is moving on ground
	bool IsCharacterMovingOnGround();
	//Check if Character is at a wall while in air.
	bool CanWallSlide();

private:	
	//**DEBUGGING**//
	void Debug(); //Shows a debug line that traces the character's 3D movements.
	// These values are for the Debug method above.
	FVector PreviousPosition;
	FVector CurrentPosition;
	
	void DebugState(); //Prints the current MOVEMENT state of the character.
	void DebugText(FString Text);
	
public:
	//*************************************************************************//
	//** IMPORTANT NOTE:Movement Values == MOVE THESE TO FMovementParameters **//
	//*************************************************************************//
	//Default Gravity Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Movement Values")
	float DefaultGravity = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Movement Values")
	float DefaultYRotationRate = 540.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Movement Values")
	float DefaultAirControl = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Values")
	float DefaultJumpZVelocity = 750.0f;
	//Jump
	bool CanJump = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	int JumpCount = 0;
	UFUNCTION(BlueprintCallable)
	void ResetJump() { CanJump = true; JumpCount = 0; };
	//Diving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	bool CanDive = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	bool isDiving = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	float DiveSpeed = 1250.0f ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	float DiveGravityScale = 1.75f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	float DiveLength = 0.25f ;
	//Wall Sliding + Wall Jumps
	bool CanWallJump = false;
	bool WallSlideStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide & Wall Jumps")
	bool IsWallSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide & Wall Jumps")
	float WallSlideGravityScale = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide & Wall Jumps")
	float WallSlideDeceleration = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide & Wall Jumps")
	float WallJumpSpeed = 750.0f;
	
	

	//Movement Methods
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	//AirJump Methods
	virtual void AirJump();

	//Diving Methods
	virtual void Dive();
	UFUNCTION(BlueprintCallable)
	virtual void ResetDive();

	//Jumping Methods
	virtual void Jump() override;
	virtual void StopJumping() override;

	//Reset Movement Values to Default Values
	void ResetToDefaults();

	// Called when Player Character has landed
	virtual void Landed(const FHitResult& Hit) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
