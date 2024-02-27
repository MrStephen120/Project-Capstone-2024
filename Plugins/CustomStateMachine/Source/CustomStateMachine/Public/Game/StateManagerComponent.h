// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/StateBase.h"
#include "StateManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMSTATEMACHINE_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	FString InitialState; //Initial State the actor will be in.
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	TMap<FString, TSubclassOf<UStateBase>> AvailableStates; //Carries a map of ALL included sub classes of UStateBase
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine Debug")
	bool bDebug = false; 

	UPROPERTY(BlueprintReadOnly)
	TArray<UStateBase*> StateHistory; //To see the history of states.
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="State Machine Debug", meta = (ClampMin = 0, ClampMax = 10, UIMin = 0, UIMax = 10))
	int32 StateHistoryLength; //History length of states called.
	
	UPROPERTY(BlueprintReadOnly)
	UStateBase* CurrentState = nullptr; //Current Active State

	UPROPERTY()
	TMap<FString, UStateBase*> StateMap;//Carries a map of ALL included sub classes of UStateBase* POINTER

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void SwitchStateByKey(FString StateKey); //Switch state by string key.
	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void InitStateManager(); //Call on BeginPlay or Initialization

private:
	bool bCanTickState = false;
	void InitializeStates();
	
};
