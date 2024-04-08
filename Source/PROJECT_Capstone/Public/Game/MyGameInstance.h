// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class AGM_Platformer;

UCLASS()
class PROJECT_CAPSTONE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//Coins Collected 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Coins")
	int Coins = 0;
	UFUNCTION(BlueprintCallable, Category="Coins")
	void SubtractCoins(int CoinsToSubtract);
	UFUNCTION(BlueprintCallable, Category="Coins")
	void AddCoins(int CoinsToAdd);

	//Trophies Collected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trophies")
	int TrophiesToWin = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trophies")
	int Trophies = 0;
	UFUNCTION(BlueprintCallable, Category="Trophies")
	void AddTrophy(int TrophyToAdd);

	// Stores the IDs of collected trophies
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trophies")
	TArray<FString> CollectedTrophiesIDs;

	// Function to add a trophy ID to the collected list
	UFUNCTION(BlueprintCallable, Category = "Trophies")
	void CollectTrophy(const FString& TrophyID);

	// Function to check if a trophy is collected
	UFUNCTION(BlueprintCallable, Category = "Trophies")
	bool IsTrophyCollected(const FString& TrophyID) const;

	// Gates
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Challenge Gates")
	TArray<FString> ChallengeGatesCompleted;

	// Function to see if trophy inside challenge gate has been collected.
	UFUNCTION(BlueprintCallable, Category = "Challenge Gates")
	void CompleteChallenge(const FString& GateID);

	// Function to check if the challenge gate has been completed.
	UFUNCTION(BlueprintCallable, Category = "Challenge Gates")
	void IsChallengeCompleted(const FString& GateID);

	
	//Win Screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface")
	TSubclassOf<UUserWidget> WinScreenWidget = nullptr;
	
	UFUNCTION(BlueprintCallable)
	void CheckIfWin();
};
