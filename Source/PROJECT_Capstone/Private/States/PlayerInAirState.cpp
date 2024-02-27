// Fill out your copyright notice in the Description page of Project Settings.


#include "States/PlayerInAirState.h"

#include "GameFramework/CharacterMovementComponent.h"

void UPlayerInAirState::PressJump()
{
	Super::PressJump();
	const FString DebugText = "Player In-Air.";
	const FColor TextColor = FColor::Purple;
	float Duration = 1.0f;
	GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, DebugText);
}

void UPlayerInAirState::TickState()
{
	Super::TickState();
	if (PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
}
