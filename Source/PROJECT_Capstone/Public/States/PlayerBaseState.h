// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Plugins/CustomStateMachine/Source/CustomStateMachine/Public/Game/StateBase.h"
#include "Character/MainCharacter.h"
#include "Game/MyPlayerControllerInterface.h"
#include "PlayerBaseState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CAPSTONE_API UPlayerBaseState : public UStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	AMainCharacter* PlayerRef = nullptr;
	IMyPlayerControllerInterface* PlayerController = nullptr;

	virtual void OnEnterState(AActor* OwnerRef) override;
	virtual void OnExitState() override;

protected:
	virtual void PressJump();
};
