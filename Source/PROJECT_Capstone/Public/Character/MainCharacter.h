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

	// Running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Running")
	float RunSpeed;

	// Jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jumping")
	float JumpHeight;

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
	DoubleJumping,
	Diving,
	LedgeGrabbing,
	WallSliding,
	WallJumping
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
	
	//Particles
	//Dust Trail
	UNiagaraComponent* WalkingParticlesComponent;
	bool CanSpawnWalkParticles;
	//Jump Dust Ring
	UNiagaraComponent* SmokeRingParticleComponent;
	bool CanSmokeRingParticles;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void UpdateMovementState();
	
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
	
	void HandleWalkParticles();
	void HandleJumpSmokeRing();

	//Getters for Camera Components
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Set up CharacterMovement Settings
	void SetUpCharacterMovementSettings();

	//Set up Camera Settings
	void SetUpCamera();

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	void AirJump();
	
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
