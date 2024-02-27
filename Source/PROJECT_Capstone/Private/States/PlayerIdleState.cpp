// Fill out your copyright notice in the Description page of Project Settings.


#include "States/PlayerIdleState.h"

#include "GameFramework/CharacterMovementComponent.h"

void UPlayerIdleState::PressJump()
{
	Super::PressJump();
	PlayerRef->ChangeState(EMovementState::Jumping);
}

void UPlayerIdleState::TickState()
{
	Super::TickState();
	if (!PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Air");
	}
}
