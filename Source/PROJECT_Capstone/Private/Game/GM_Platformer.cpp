// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GM_Platformer.h"
#include "Character/MainCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGM_Platformer::AGM_Platformer()
{
    DefaultPawnClass = AMainCharacter::StaticClass();

}
