// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "States/PlayerBaseState.h"
#include "PlayerIdleState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CAPSTONE_API UPlayerIdleState : public UPlayerBaseState
{
	GENERATED_BODY()
protected:
	virtual void PressJump() override;
	virtual void TickState() override;
};
