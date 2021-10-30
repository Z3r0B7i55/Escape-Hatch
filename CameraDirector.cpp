// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	FirstSpawn = false;
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeBetweenCameraChange = 3.f;
	const float SmoothBlendTime = .75f;
	TimeToNextCameraChange -= DeltaTime;

	// Find the actor that handles control for the local player.
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (OurPlayerController && !FirstSpawn) // Once the cycle is complete, FirstSpawn will be set to true, disabling this.
	{
		if (TimeToNextCameraChange <= 0.0f)
		{
			// Time - currentTime + time between camera change so 3 seconds.
			TimeToNextCameraChange += TimeBetweenCameraChange;

			// If CameraTwo is a real thing and the current viewport is camera one.
			if (CameraTwo && (OurPlayerController->GetViewTarget() == CameraOne))
			{
				OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
				FirstSpawn = true; // Cycle complete
			}

			//Other wise, if camera one is a real thing.
			else if (CameraOne)
			{
				// Initiate cycle, switch to Camera one to start.
				OurPlayerController->SetViewTarget(CameraOne);
			}
		}
	}
}

