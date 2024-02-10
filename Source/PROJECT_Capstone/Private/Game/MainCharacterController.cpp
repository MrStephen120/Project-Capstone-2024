// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainCharacterController.h"
#include "Character/MainCharacter.h"
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
	////Find Input Actions in Content Browser
	////Move Action
	//static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionObject(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move'"));
	//if (MoveActionObject.Succeeded())
	//{
	//	MoveAction = MoveActionObject.Object;
	//	UE_LOG(MainCharacterController, Log, TEXT("Found MoveActionObject"));
	//}
	//else {
	//	UE_LOG(MainCharacterController, Warning, TEXT("MoveActionObject not found"));
	//}
	////Look Action
	//static ConstructorHelpers::FObjectFinder<UInputAction> LookActionObject(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look'"));
	//if (LookActionObject.Succeeded())
	//{
	//	LookAction = LookActionObject.Object;
	//	UE_LOG(MainCharacterController, Log, TEXT("Found LookActionObject"));
	//}
	//else {
	//	UE_LOG(MainCharacterController, Warning, TEXT("LookActionObject not found"));
	//}
	////Jump Action
	//static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionObject(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump'"));
	//if (JumpActionObject.Succeeded())
	//{
	//	JumpAction = JumpActionObject.Object;
	//	UE_LOG(MainCharacterController, Log, TEXT("Found JumpActionObject"));
	//}
	//else {
	//	UE_LOG(MainCharacterController, Warning, TEXT("JumpActionObject not found"));
	//}
	////Input Mapping Context
	//static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCObject(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_Default.IMC_Default'"));
	//if (IMCObject.Succeeded())
	//{
	//	InputMappingContext = IMCObject.Object;
	//	UE_LOG(MainCharacterController, Log, TEXT("Found IMCObject"));
	//}
	//else {
	//	UE_LOG(MainCharacterController, Warning, TEXT("IMCObject not found"));
	//}

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
}

void AMainCharacterController::OnUnPossess()
{
	// Unbind things here...
	EnhancedInput->ClearActionBindings();

	// Call the parent method, in case it needs to do anything.
	Super::OnUnPossess();
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
	// Input is 'Digital' (value not used here)
	// Make the Player's Character Pawn jump, disabling crouch if it was active
	if (PlayerCharacter)
	{
		if (JumpCount < PlayerCharacter->JumpMaxCount) {

			if (!PlayerCharacter->GetCharacterMovement()->IsFalling()) {
				//Ground Jump
				PlayerCharacter->Jump();
				++JumpCount;
				UE_LOG(MainCharacterController, Log, TEXT("JumpCount value: %d"), JumpCount);
			}
			//For Future Use: Check if it's a wall jump, if false then it's an air jump.

			else { //AirJump
				PlayerCharacter->AirJump();
				++JumpCount;
				UE_LOG(MainCharacterController, Log, TEXT("JumpCount value: %d"), JumpCount);
			}
		}
	}
}

void AMainCharacterController::HandleStopJumping()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->StopJumping();
	}
}

void AMainCharacterController::HandleLookAction(const FInputActionValue& Value)
{
	// Input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Add yaw and pitch input to controller
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}


