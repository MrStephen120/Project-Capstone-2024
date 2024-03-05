// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>

#include <CoreMinimal.h>
#include "MainCharacterMovement.generated.h"

UCLASS()
class PROJECT_CAPSTONE_API UMainCharacterMovement : public UCharacterMovementComponent
{
private:
	GENERATED_BODY()
    bool bIsJumpHeld;
    float JumpKeyHoldTime;  // How long the jump key can be held
    float JumpMaxHoldTime;  // Maximum time the jump key can be held
    float StickyFeetFriction;  // Increased friction on landing

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction) override;
	virtual bool DoJump(bool isReplayingMoves) override;
	virtual bool IsFalling() const override;

public:
};
