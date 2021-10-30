// Fill out your copyright notice in the Description page of Project Settings.

#include "OnHitTrigger.h"
#include "Components/BoxComponent.h"
#include "Engine.h"

// Sets default values
AOnHitTrigger::AOnHitTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a box as simple collision representation.
	MyComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	MyComp->SetSimulatePhysics(true);
	MyComp->SetNotifyRigidBodyCollision(true);

	MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	MyComp->OnComponentHit.AddDynamic(this, &AOnHitTrigger::OnCompHit);

	// Set as root component.
	RootComponent = MyComp;
}

// Called when the game starts or when spawned
void AOnHitTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOnHitTrigger::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{
	if(OtherActor && (OtherActor != this) && OtherComp) 
	{ // Avoid null actor        avoid this actor		avoid null hits

		if(LastHit == OtherActor) return; //To avoid repeat messages.

					// Debug output of detected object, pawn or character's name.
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Collided with %s"), *OtherActor->GetName()));
		LastHit = OtherActor; // To Avoid repeat messages.
	}
}

// Called every frame
void AOnHitTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}