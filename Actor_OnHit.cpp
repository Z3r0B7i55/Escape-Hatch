// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_OnHit.h"

// Sets default values
AActor_OnHit::AActor_OnHit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_OnHit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_OnHit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AActor_OnHit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

