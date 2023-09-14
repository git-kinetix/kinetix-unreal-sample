// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealKinetixSampleGameMode.h"
#include "UnrealKinetixSampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealKinetixSampleGameMode::AUnrealKinetixSampleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_UnrealKinetixSampleCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
