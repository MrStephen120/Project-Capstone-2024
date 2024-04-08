// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputAction.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Character/MainCharacter.h"
#include "Game/MyPlayerControllerInterface.h"
#include "MainCharacterController.generated.h"

// Forward Declarations
class UEnhancedInputComponent;
class AMainCharacter;
class UInputMappingContext;

DECLARE_LOG_CATEGORY_EXTERN(MainCharacterController, Log, All);
UCLASS(Abstract)
class PROJECT_CAPSTONE_API AMainCharacterController : public APlayerController, public IMyPlayerControllerInterface
{

	GENERATED_BODY()

private:	FJumpSignature JumpDelegate; 
protected:	virtual FJumpSignature* GetJumpDelegate() override; //If anyone gets the JumpDelegate through this method, it will know if player pressed Jump.
	
public:
	// Enhanced Input Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Action")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Action")
	TObjectPtr<UInputAction> LookAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reset Camera Action")
	TObjectPtr<UInputAction> ResetCameraAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dive Action")
	TObjectPtr<UInputAction> DiveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSlide Action")
	TObjectPtr<UInputAction> WallSlideAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Navigation")
	TObjectPtr<UInputAction> UINavigateAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Navigation")
	TObjectPtr<UInputAction> UIConfirmAction = nullptr;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	//Pause Menu UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Pause")
	TSubclassOf<UUserWidget> PauseWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* PauseWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Pause")
	TObjectPtr<UInputAction> UIPauseAction = nullptr;
	
protected:
	// Enhanced Input Action Handlers
	//void Move();
	void HandleMoveAction(const FInputActionValue& Value);

	//void Jump();
	void HandleJumpAction();
	void HandleStopJumping();

	//void Dive();
	void HandleDiveAction();

	//void WallSlide();
	void HandleWallSlideAction();
	void HandleStopWallSlideAction();
	
	//void Look();
	void HandleLookAction(const FInputActionValue& Value);

	//void ResetCamera();
	void HandleResetCameraAction();

	//Pause Game
	void HandlePauseGame();
	
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
