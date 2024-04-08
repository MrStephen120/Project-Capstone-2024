// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MyGameInstance.h"

#include "Blueprint/UserWidget.h"

void UMyGameInstance::SubtractCoins(int CoinsToSubtract)
{
	Coins -= CoinsToSubtract;

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Coins Collected: %i"), Coins));	
}

void UMyGameInstance::AddCoins(int CoinsToAdd)
{
	Coins += CoinsToAdd;
	//Play Effects Here
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Coins Collected: %i"), Coins));
}

void UMyGameInstance::AddTrophy(int TrophyToAdd)
{
	Trophies += TrophyToAdd;
	CheckIfWin();
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Trophies Collected: %i"), Trophies));
}

void UMyGameInstance::CheckIfWin()
{
	UUserWidget* WinScreen = CreateWidget<UUserWidget>(GetWorld(), WinScreenWidget);
	if (Trophies >= TrophiesToWin)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Game Win");
		//Pop up the Win Screen
		WinScreen->AddToViewport();
		GetWorld()->GetFirstPlayerController()->SetPause(true);
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}
}
