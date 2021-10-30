// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class GAM312_FINAL_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Cameras)
		AActor* CameraOne;

	UPROPERTY(EditAnywhere, Category = Cameras)
		AActor* CameraTwo;

	/** 
		FirstSpawn is set to true after the camera swaps to the 
		players FirstPersonCamera. Meaning, after BeginPlay();
		and after the camera makes a 180 degree rotation 
		FirstSpawn will be set to true.
	*/
	bool FirstSpawn = false;

	float TimeToNextCameraChange;
};
