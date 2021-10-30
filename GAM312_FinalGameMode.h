// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GAM312_FinalCharacter.h"
#include "GAM312_FinalGameMode.generated.h"

UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class AGAM312_FinalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGAM312_FinalGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	AGAM312_FinalCharacter* MyCharacter;

	/** Sets a new Playing State */
	UFUNCTION(BlueprintPure, Category = "Health")
	EGamePlayState GetCurrentState() const;

	/** Set a new Playing State */
	void SetCurrentState(EGamePlayState NewState);

private:
	/** Keep track of the current playing state */
	EGamePlayState CurrentState;

	/** Handle any function calls that rely on changing the playing state of the game. */
	void HandleNewState(EGamePlayState NewState);
};



