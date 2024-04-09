// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GM_Platformer.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGM_Platformer::AGM_Platformer()
{
    DefaultPawnClass = AMainCharacter::StaticClass();
    PlayerControllerClass = AMainCharacterController::StaticClass();
}

void AGM_Platformer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGM_Platformer::BeginPlay()
{
    Super::BeginPlay();
    //Initialize User Interface
    InitializeUserInterface();
    
    //Get CHaracter
    PlayerCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    checkf(PlayerCharacter, TEXT("GM_Platformer - Failed to cast to PlayerCharacter."))
    
    //Bind OnDestroyedEvent
    if (PlayerCharacterClass)
    {
        //Bind to OnDestroyed Event
        PlayerCharacter->OnDestroyed.AddDynamic(this, &AGM_Platformer::OnCharacterDestroyed);
    }
}
void AGM_Platformer::InitializeUserInterface()
{
   UUserWidget* CoinsUI = CreateWidget<UUserWidget>(GetWorld(), CoinsIndicatorClass);
    if (CoinsUI != nullptr)
    {
        CoinsUI->AddToViewport();
    }
}

void AGM_Platformer::OnCharacterDestroyed(AActor* DestroyedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter was destroyed"));
    // Set the timer to call the function after a delay
    GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AGM_Platformer::RespawnCharacter, 1.0f, false);
}

void AGM_Platformer::RespawnCharacter()
{
    // Ensure we have a valid reference to the character class
    if (PlayerCharacterClass != nullptr)
    {
        //Spawn the new Character
        AMainCharacter* NewCharacter = GetWorld()->SpawnActor<AMainCharacter>(PlayerCharacterClass, SpawnTransform);

        // Possess the newly spawned character with the player's controller
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PlayerController && NewCharacter)
        {
            PlayerController->Possess(NewCharacter);
        }

        //MIGHT BE REDUNDANT.
        //Get new Character
        PlayerCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        checkf(PlayerCharacter, TEXT("GM_Platformer - Failed to cast to PlayerCharacter."))
    
        //Bind OnDestroyedEvent
        if (PlayerCharacterClass)
        {
            //Bind to OnDestroyed Event
            PlayerCharacter->OnDestroyed.AddDynamic(this, &AGM_Platformer::OnCharacterDestroyed);
        }
    }
}

