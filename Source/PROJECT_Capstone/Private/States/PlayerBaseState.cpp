// Fill out your copyright notice in the Description page of Project Settings.


#include "States/PlayerBaseState.h"
#include "Game/MyPlayerControllerInterface.h"
#include "Kismet/GameplayStatics.h"

void UPlayerBaseState::OnEnterState(AActor* OwnerRef)
{
	Super::OnEnterState(OwnerRef);
	//Save Player Ref for later
	if (!PlayerRef)
	{
		PlayerRef = Cast<AMainCharacter>(OwnerRef);
	}

	//*Get Player Controller
	if (!PlayerController)
	{
		Cast<IMyPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	}
	//*Bind Jump Delegate
	if (PlayerController)
	{
		PlayerController->GetJumpDelegate()->AddUObject(this, &UPlayerBaseState::PressJump);
	}
}

void UPlayerBaseState::OnExitState()
{
	Super::OnExitState();
	//*Remove Jump Delegate
	PlayerController->GetJumpDelegate()->RemoveAll(this);
}

void UPlayerBaseState::PressJump()
{
}
