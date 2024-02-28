// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Character.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "MainCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MainCharacter, Log, All);

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
	
	//SetUp Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//Current movement state of the character
	EMovementState CurrentState;
	
	//For Debug Line
	FVector PreviousPosition;
	FVector CurrentPosition;
	
	//Particles
	//Dust Trail
	UNiagaraComponent* WalkingParticlesComponent;
	bool CanSpawnWalkParticles;
	//Jump Dust Ring
	UNiagaraComponent* SmokeRingParticleComponent;
	bool CanSmokeRingParticles;
	
	//Animation Instance
	UAnimInstance* AnimInstance;
	FBoolProperty* bDiveAnimProperty;
public:
	//Changes State of Player Character
	void ChangeState(EMovementState NewState);
	
protected:
	//Updates State on Tick
	void UpdateState();
	//==|State Specific Methods|==
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
	
	//Check if Character has no inputs and velocity = 0;
	bool IsCharacterIdle();
	//Check if Character is grounded
	bool IsCharacterGrounded();
	//Check if Character is moving on ground
	bool IsCharacterMovingOnGround();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	//Particle
	//Dust Trail
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* WalkSmokeTrail;
	//Jump Dust Ring
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* JumpSmokeRing;

	// Sets default values for this character's properties
	AMainCharacter(const FObjectInitializer& object);

	//Particle Methods
	void HandleWalkParticles();
		void ActivateWalkParticles();
		void DeActivateWalkParticles();
	
	void HandleJumpSmokeRing();

	//Getters for Camera Components
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Set up CharacterMovement Settings
	void SetUpCharacterMovementSettings();

	//Set up Camera Settings
	void SetUpCamera();
	
	//For Debug
	void DebugState();
	void DebugText(FString Text);
	
public:
	//Movement Values
	//Default Gravity Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Values")
	float DefaultGravity = 2.0f;
	//Jump
	bool CanJump = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Values")
	int JumpCount = 0;
	UFUNCTION(BlueprintCallable)
	void ResetJump() { CanJump = true; JumpCount = 0; };
	//Diving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	float DiveSpeed = 1000.0f ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diving")
	float DiveGravityScale = 0.0f;
	
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	void AirJump();
	
	virtual void Dive();
	
	virtual void Jump() override;

	virtual void StopJumping() override;

	virtual void Landed(const FHitResult& Hit) override;
	//For Debug
	void Debug();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
