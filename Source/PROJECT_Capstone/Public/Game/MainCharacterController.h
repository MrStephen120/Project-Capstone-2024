// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputAction.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MainCharacterController.generated.h"

// Forward Declarations
class UEnhancedInputComponent;
class AMainCharacter;
class UInputMappingContext;

DECLARE_LOG_CATEGORY_EXTERN(MainCharacterController, Log, All);
UCLASS(Abstract)
class PROJECT_CAPSTONE_API AMainCharacterController : public APlayerController
{

	GENERATED_BODY()

public:
	// Enhanced Input Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputAction> LookAction = nullptr; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Jump")
	int JumpCount;

	UFUNCTION(BlueprintCallable)
	void ResetJump() { JumpCount = 0; };

protected:
	// Enhanced Input Action Handlers
	//void Move();
	void HandleMoveAction(const FInputActionValue& Value);

	//void Jump();
	void HandleJumpAction();
	void HandleStopJumping();

	//void Look();
	void HandleLookAction(const FInputActionValue& Value);

	//Use FInputActionInstance for more details on input being handled.

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

private:

	//Store reference to InputComponent to cast to EnhancedInputComponent
	UPROPERTY() 
	TObjectPtr<UEnhancedInputComponent> EnhancedInput = nullptr;
	//Store reference to the pawn being controlled;
	UPROPERTY()
	TObjectPtr<AMainCharacter> PlayerCharacter = nullptr;
};
