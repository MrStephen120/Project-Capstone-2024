// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MyGameInstance.h"

#include "Blueprint/UserWidget.h"

void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::SubtractCoins(int CoinsToSubtract)
{
	if (Coins - CoinsToSubtract < 0)
	{
		Coins = 0;
	}
	else
	{
		Coins -= CoinsToSubtract;
	}

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

void UMyGameInstance::CollectTrophy(const FString& TrophyID)
{
	if (!CollectedTrophiesIDs.Contains(TrophyID))
	{
		CollectedTrophiesIDs.Add(TrophyID);
	}
}

bool UMyGameInstance::IsTrophyCollected(const FString& TrophyID) const
{
	return CollectedTrophiesIDs.Contains(TrophyID);
}

void UMyGameInstance::CompleteChallenge(const FString& GateID)
{
	if (!ChallengeGatesCompleted.Contains(GateID))
	{
		ChallengeGatesCompleted.Add(GateID);
	}
}

bool UMyGameInstance::IsChallengeCompleted(const FString& GateID)
{
	return ChallengeGatesCompleted.Contains(GateID);
}

void UMyGameInstance::CheckIfWin()
{
	UUserWidget* WinScreen = CreateWidget<UUserWidget>(GetWorld(), WinScreenWidget);
	if (Trophies >= TrophiesToWin)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Game Win");
		//Stop Timer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		//Pop up the Win Screen
		WinScreen->AddToViewport();
		GetWorld()->GetFirstPlayerController()->SetPause(true);
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}
}

void UMyGameInstance::StartTimer()
{
	//Start Timer
	if (!isTimerRunning)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMyGameInstance::UpdateTimer, 1.0f, true);
		isTimerRunning = true;
	}

}

void UMyGameInstance::PauseTimer()
{
	if (!GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle))
	{
		GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
	}
	else
	{
		GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
	}
}

void UMyGameInstance::StopTimer()
{
	if (isTimerRunning)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		isTimerRunning = false;
	}
}

void UMyGameInstance::ResetTimer()
{
	StopTimer();
	TotalSecondsElapsed = 0;
}

FString UMyGameInstance::GetFormattedTime()
{
	int Seconds = TotalSecondsElapsed % 60;
	int Minutes = (TotalSecondsElapsed % 3600) / 60;
	
	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

