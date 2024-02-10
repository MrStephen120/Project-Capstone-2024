// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacterMovement.h"

#include "Character/MainCharacter.h"
#include "Components/CapsuleComponent.h"

//For debugging
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void UMainCharacterMovement::BeginPlay()
{
}

void UMainCharacterMovement::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction)
{
}

bool UMainCharacterMovement::DoJump(bool isReplayingMoves)
{
	return false;
}

bool UMainCharacterMovement::IsFalling() const
{
	return false;
}
