// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MainCharacter, Log, All);

UCLASS()
class PROJECT_CAPSTONE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

	//SetUp Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AMainCharacter(const FObjectInitializer& object);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//Getters for Camera Components
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Set up CharacterMovement Settings
	void SetUpCharacterMovementSettings();

	//Set up Camera Settings
	void SetUpCamera();

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	virtual void Jump() override;

	virtual void StopJumping() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
