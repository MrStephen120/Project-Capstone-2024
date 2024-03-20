// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(MainCharacterController);

void AMainCharacterController::OnPossess(APawn* aPawn)
{
	//Parent method
	Super::OnPossess(aPawn);

	//Store reference to Player's Pawn
	PlayerCharacter = Cast<AMainCharacter>(aPawn);
	checkf(PlayerCharacter, TEXT("AMainCharacterController derived classes should only posess AMainCharacter derived pawns"));

	// Get a reference to the EnhancedInputComponent
	EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInput, TEXT("Unable to get reference to the EnhancedInput Component."));

	// Get the local player subsystem
	// Just a local variable, we dont need to refer to it again after this
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem."));

	// Wipe existing mappings, and add our mapping.
	checkf(InputMappingContext, TEXT("InputMappingContext was not specified."));
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);
	
	//Bind Input Actions
	//Attempts to bind if a valid value was provided.
	//Move
	if (MoveAction) {
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacterController::HandleMoveAction);
	}
	//Look
	if (LookAction) {
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacterController::HandleLookAction);
	}
	//Jump
	if (JumpAction) {
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMainCharacterController::HandleJumpAction);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMainCharacterController::HandleStopJumping);
	}
	//Dive
	if (DiveAction)
	{
		EnhancedInput->BindAction(DiveAction, ETriggerEvent::Started, this, &AMainCharacterController::HandleDiveAction);
	}
	//Jump
	if (WallSlideAction) {
		EnhancedInput->BindAction(WallSlideAction, ETriggerEvent::Started, this, &AMainCharacterController::HandleWallSlideAction);
		EnhancedInput->BindAction(WallSlideAction, ETriggerEvent::Completed, this, &AMainCharacterController::HandleStopWallSlideAction);
	}

	if (ResetCameraAction)
	{
		EnhancedInput->BindAction(ResetCameraAction, ETriggerEvent::Started, this, &AMainCharacterController::HandleResetCameraAction);
	}
}

void AMainCharacterController::OnUnPossess()
{
	// Unbind things here...
	EnhancedInput->ClearActionBindings();

	// Call the parent method, in case it needs to do anything.
	Super::OnUnPossess();
}

FJumpSignature* AMainCharacterController::GetJumpDelegate()
{
	return &JumpDelegate;
}

void AMainCharacterController::HandleMoveAction(const FInputActionValue& Value)
{
	// Value is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Add movement to the Player's Character Pawn
	if (PlayerCharacter)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Get the axis direction
		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Move the player to the specified direction.
		PlayerCharacter->AddMovementInput(DirectionY, MovementVector.X);
		PlayerCharacter->AddMovementInput(DirectionX, MovementVector.Y);
	}
}

void AMainCharacterController::HandleJumpAction()
{
	PlayerCharacter->HandleJumpRequest();
}

void AMainCharacterController::HandleStopJumping()
{
	PlayerCharacter->StopJumping();
}

void AMainCharacterController::HandleDiveAction()
{
	PlayerCharacter->ChangeState(EMovementState::Diving);
}

void AMainCharacterController::HandleWallSlideAction()
{
	PlayerCharacter->bCanWallSlide = true;
}

void AMainCharacterController::HandleStopWallSlideAction()
{
	PlayerCharacter->bCanWallSlide = false;
}

void AMainCharacterController::HandleLookAction(const FInputActionValue& Value)
{
	// Input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Add yaw and pitch input
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
	
	PlayerCharacter->SetCameraDistance(FMath::Clamp(((-LookAxisVector.Y * -30.0f) + PlayerCharacter->CameraDistance), PlayerCharacter->MinCameraDistance, PlayerCharacter->MaxCameraDistance));
}

void AMainCharacterController::HandleResetCameraAction()
{
	PlayerCharacter->ResetCamera();
}


