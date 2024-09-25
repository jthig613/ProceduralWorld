// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralTerrainGameMode.h"
#include "ProceduralTerrainCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProceduralTerrainGameMode::AProceduralTerrainGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
