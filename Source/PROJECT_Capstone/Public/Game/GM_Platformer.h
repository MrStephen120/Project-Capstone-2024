// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Character/MainCharacter.h"
#include "Game/MainCharacterController.h"
#include "GM_Platformer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CAPSTONE_API AGM_Platformer : public AGameModeBase
{
	GENERATED_BODY()
private:
	//Store reference to the pawn being controlled;
	UPROPERTY()
	TObjectPtr<AMainCharacter> PlayerCharacter = nullptr;
	
public:
	//Constructor
	AGM_Platformer();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Handler for OnDestroyed Event
	UFUNCTION()
	void OnCharacterDestroyed(AActor* DestroyedActor);
	
	//Handle for Respawning Character, Called after DelayTimer
	UFUNCTION()
	void RespawnCharacter();
	
	//Coins Collected 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Coins")
	int Coins = 0;
	UFUNCTION(BlueprintCallable, Category="Coins")
	void SubtractCoins(int CoinsToSubtract);
	UFUNCTION(BlueprintCallable, Category="Coins")
	void AddCoins(int CoinsToAdd);

	//Trophies Collected
	//Coins Collected 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trophies")
	int Trophies = 0;
	UFUNCTION(BlueprintCallable, Category="Trophies")
	void AddTrophy(int TrophyToAdd);
protected:
    //Timer handle for delay
    FTimerHandle DelayTimerHandle;

	//User Interface References 
	void InitializeUserInterface();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface")
	TSubclassOf<UUserWidget> CoinsIndicatorClass;
	
	//The spawn transform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawning")
	FTransform SpawnTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	TSubclassOf<AMainCharacter> PlayerCharacterClass = nullptr;
};
