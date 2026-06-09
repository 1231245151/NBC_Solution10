// Copyright Epic Games, Inc. All Rights Reserved.

#include "NBC_Solution10GameMode.h"
#include "NBC_Solution10Character.h"
#include "UObject/ConstructorHelpers.h"

ANBC_Solution10GameMode::ANBC_Solution10GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
