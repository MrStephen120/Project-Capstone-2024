// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainCharacterController.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(MainCharacterController);

void AMainCharacterController::OnPossess(APawn* aPawn)
{
	//Parent method
	Super::OnPossess(aPawn);
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

	if (EnhancedInput)
	{
		//Bind EIC Actions
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacterController::HandleMoveAction);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacterController::HandleLookAction);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMainCharacterController::HandleJumpAction);
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
}

void AMainCharacterController::HandleJumpAction(const FInputActionValue& Value)
{
}

void AMainCharacterController::HandleLookAction(const FInputActionValue& Value)
{
}


