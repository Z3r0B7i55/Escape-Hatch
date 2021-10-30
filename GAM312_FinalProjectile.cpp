// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312_FinalProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Blueprint.h"
#include "GameFramework/ProjectileMovementComponent.h"

AGAM312_FinalProjectile::AGAM312_FinalProjectile() 
{
	// Use a sphere as a simple collision representation
	static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint(TEXT("Blueprint'/Game/Blueprints/BP_bullet'"));
	if (ItemBlueprint.Object)
	{
		prototypeBullet = (UClass *)ItemBlueprint.Object->GeneratedClass;
	}

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->Buoyancy = 1.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}
