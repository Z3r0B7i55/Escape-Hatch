// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312_FinalGameMode.h"
#include "GAM312_FinalHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GAM312_FinalCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGAM312_FinalGameMode::AGAM312_FinalGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGAM312_FinalHUD::StaticClass();
}

void AGAM312_FinalGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	// Cause to Main Character to obtain their health.
	MyCharacter = Cast<AGAM312_FinalCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AGAM312_FinalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld() -> GetMapName();

	if (MyCharacter)
	{
		//When main character looses all their health, SetCurrentState to EGameOver.
		if (FMath::IsNearlyZero(MyCharacter -> GetHealth(), 0.001f)) 
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState AGAM312_FinalGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AGAM312_FinalGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

// Respawn character upon death I.E. EGamePlayState EGameOver.
void AGAM312_FinalGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
		/* Playing Game State */
		case EGamePlayState::EPlaying:
		{
		}
		break;

		/* Game Over Game State */
		case EGamePlayState::EGameOver:
		{
			UGameplayStatics::OpenLevel(this, FName(*GetWorld() -> GetName()), false);
		}
		break;

		/* Unknown Game State */
		case EGamePlayState::EUnknown:
		{
		}
		break;
	}
}