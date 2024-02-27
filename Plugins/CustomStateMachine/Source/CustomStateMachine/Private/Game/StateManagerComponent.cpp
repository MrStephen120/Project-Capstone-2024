// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/StateManagerComponent.h"

// Sets default values for this component's properties
UStateManagerComponent::UStateManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeStates();
}


// Called every frame
void UStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bCanTickState)
	{
		CurrentState->TickState();
	}
	
	if (bDebug) //Debugging
	{
		if (CurrentState)//Prints CurrentState owner is in.
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s current state: " + CurrentState->StateDisplayName.GetPlainNameString());
		
		if (StateHistory.Num()>0)//Prints all previous states owner was in.
		{
			for (int32 i = 0; i<StateHistory.Num(); i++)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, this->GetOwner()->GetName() + "'s past state "+ FString::FromInt(i)+" " + StateHistory[i]->GetName());
			}
		}	
	}
}

void UStateManagerComponent::SwitchStateByKey(FString StateKey)
{
	//BindState
	UStateBase* NewState = StateMap.FindRef(StateKey);
	if (NewState->IsValidLowLevel())
	{
		//If there is no CurrentState, we are at initialization.
		if (!CurrentState)
		{
			CurrentState = NewState;//Set CurrentState to NewState
		}
		else
		{
			//Calls when the State is non-repeatable.
			if (CurrentState->GetClass() == NewState->GetClass() && CurrentState->bCanRepeat == false)
			{
				if (bDebug)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed. " + CurrentState->StateDisplayName.GetPlainNameString() + " is not repeatable!", true);
				}
			}
			else
			{
				bCanTickState = false; //Stops all ticking
				CurrentState->OnExitState();//Exits Current State
				
				if(StateHistory.Num()<StateHistoryLength)//Add Current State into StateHistory
				{
					StateHistory.Push(CurrentState);
				}
				else 
				{
					StateHistory.RemoveAt(0);
					StateHistory.Push(CurrentState);
				}

				CurrentState = NewState;//Set CurrentState to NewState
			}
		}
		//Initialize the new State
		if (CurrentState)
		{
			CurrentState->OnEnterState(GetOwner());
			bCanTickState = true;
		}
	}
	else
	{
		//Calls when State switch fails and/or invalid state.
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed. " + "Invalid state!");
	}
}

void UStateManagerComponent::InitStateManager()
{
	SwitchStateByKey(InitialState);
}

void UStateManagerComponent::InitializeStates()
{
	//Create State and hold them in memory for when needed
	for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
	{
		UStateBase* State = NewObject<UStateBase>(this, It->Value);
		StateMap.Add(It->Key, State);
	}
}

