// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312_FinalCharacter.h"
#include "GAM312_FinalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Engine.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGAM312_FinalCharacter

AGAM312_FinalCharacter::AGAM312_FinalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	/* Because tracking the physical result of collisons with complicated masses would be too much
	for the compiler to handle, we use a capsule component for physical collisions with characters
	and pawns. As a part of that, we make the Capsule component the parent of this actor. */

	// Create a FirstPersonCameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a mesh component that will be used when being viewed from a '3rd person' view (when controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->RelativeRotation = FRotator(-10.f, 0, 90.f);
	Mesh3P->RelativeLocation = FVector(313.53f, -4.4f, -100.f);

	// Create a ThirdPersonCameraComponent	
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(Mesh3P);
	ThirdPersonCameraComponent->RelativeLocation = FVector(-300.f, 0, 104.f); // Position the camera
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AGAM312_FinalCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FullHealth = 100.f;
	Health = FullHealth; // MaxHealth and Health set to 100
	HealthPercentage = 1.0f; // Used for the healthBar Widget
	PreviousHealth = HealthPercentage; // This is for the smoothness effect.
	bCanBeDamaged = true; // Breif invincibility after damage has been caused.

	FullAmmo = 25;
	Ammo = FullAmmo; // MaxAmmo and Ammo set to 25. Pretend it's a scout rifle.
	AmmoPercentage = 1.0f; // Used for a gunBar Widget if we had one. Still the option remains.
	PreviousAmmo = AmmoPercentage; // Smoothness effect, once more.
	AmmoValue = 0.0f; 
	bCanUseAmmo = true; // Breif delay for loading next bullet.

	if(AmmoCurve) // The ammo curve relates to fire time and links to our overheat material.
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetAmmoValue"));
		TimelineFinishedCallback.BindUFunction(this, FName{TEXT("SetAmmoState")});
		MyTimeline.AddInterpFloat(AmmoCurve, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

	// Bind any recieved damage to the TakeDamage event.
	OnTakeAnyDamage.AddDynamic(this, &AGAM312_FinalCharacter::TakeDamage);

	
	/** Enable FirstPerson mode, and hide the full character, just leaving the arms and gun.*/
	FirstPerson = true;
	Mesh3P->SetOwnerNoSee(true);

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

void AGAM312_FinalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGAM312_FinalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGAM312_FinalCharacter::OnFire);

	// Bind Camera Toggle event
	PlayerInputComponent->BindAction("ToggleCamera", IE_Pressed, this, &AGAM312_FinalCharacter::ToggleCamera);
	
	//Bind Display Raycast
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AGAM312_FinalCharacter::DisplayRaycast);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGAM312_FinalCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGAM312_FinalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGAM312_FinalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGAM312_FinalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGAM312_FinalCharacter::LookUpAtRate);
}

void AGAM312_FinalCharacter::DisplayRaycast()
{
	FHitResult* HitResult = new FHitResult(); // Declare a vessel for the HitResult.

	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation(); // Returns the start vector using the postion of FirstPersonCameraComponent.
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector(); // Returns the forward vector using the rotation of FirstPersonCameraComponent.
	FVector EndTrace = ((ForwardVector * 3319.f) + StartTrace); 

	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams)) // This will return a bool, False for failure.
	{
		if (HitResult->GetActor()) // Determines if target is an actor, not BSP. Thanks Professor, for the fix.
		{
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 255, 128), false, 10.f); // Draws a debug line for 10 seconds.
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Ray traced to: %s"), *HitResult->Actor->GetName()));
		}
	}
}

void AGAM312_FinalCharacter::ToggleCamera()
{
	if (FirstPerson)
	{
		// Disable First Person Mode.
		FirstPerson = false;

		// Hide gun and arms, show body.
		FP_Gun->SetOwnerNoSee(true);
		Mesh1P->SetOwnerNoSee(true);
		Mesh3P->SetOwnerNoSee(false);

		// Switch from first to third person camera.
		FirstPersonCameraComponent->Deactivate();
		ThirdPersonCameraComponent->Activate();
	}

	else
	{
		// Enable First Person Mode.
		FirstPerson = true;

		// Hide body, show gun and arms.
		FP_Gun->SetOwnerNoSee(false);
		Mesh1P->SetOwnerNoSee(false);
		Mesh3P->SetOwnerNoSee(true);

		// Switch from third to first person camera.
		ThirdPersonCameraComponent->Deactivate();
		FirstPersonCameraComponent->Activate();
	}
}

void AGAM312_FinalCharacter::OnFire()
{
	/** If the player is in ThirdPersonView, disable OnFire();*/
	if (!FirstPerson) return;

	// try and fire a projectile
	if (ProjectileClass != NULL && !FMath::IsNearlyZero(Ammo, 0.001f) && bCanUseAmmo)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AGAM312_FinalProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AGAM312_FinalProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance *AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
		
		SetAmmoChange(-1.f); //Set Ammo Change, because this is a bullet system, 1 bullet.
	}
}

void AGAM312_FinalCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGAM312_FinalCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGAM312_FinalCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AGAM312_FinalCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGAM312_FinalCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGAM312_FinalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGAM312_FinalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AGAM312_FinalCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGAM312_FinalCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGAM312_FinalCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGAM312_FinalCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

/* Return Health and Ammo Percentage */
float AGAM312_FinalCharacter::GetHealth()
{
	return HealthPercentage;
}

float AGAM312_FinalCharacter::GetAmmo()
{
	return AmmoPercentage;
}

/* Return the FText for Health and Ammo */
FText AGAM312_FinalCharacter::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText AGAM312_FinalCharacter::GetAmmoIntText()
{
	int32 AP = FMath::RoundHalfFromZero(AmmoPercentage * FullAmmo);
	FString APS = FString::FromInt(AP);
	FString FullAPS = FString::FromInt(FullAmmo);
	FString AmmoHUD = APS + FString(TEXT("/")) + FullAPS;
	FText APText = FText::FromString(AmmoHUD);
	return APText;
}

/* Player Invincibility State */
void AGAM312_FinalCharacter::SetDamageState()
{
	bCanBeDamaged = true;
}

void AGAM312_FinalCharacter::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AGAM312_FinalCharacter::SetDamageState, 2.0f, false);
}

/* Ammo Functions */
void AGAM312_FinalCharacter::SetAmmoValue()
{
	TimelineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousAmmo + AmmoValue * AmmoCurve -> GetFloatValue(TimelineValue);
	AmmoPercentage = CurveFloatValue;
	AmmoPercentage = FMath::Clamp(AmmoPercentage, 0.f, 1.f);
}

void AGAM312_FinalCharacter::SetAmmoState()
{
	bCanUseAmmo = true;
	AmmoValue = 0.0;
	if(GunDefaultMaterial)
	{
		FP_Gun->SetMaterial(0, GunDefaultMaterial);
	}
}

/* Flash Screen on Damage */
bool AGAM312_FinalCharacter::PlayFlash()
{
	if(redFlash)
	{
		redFlash = false;
		return true;
	}
	return false;
}

/* ReceiveDamage and UpdateHealth */
void AGAM312_FinalCharacter::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	bCanBeDamaged = false; 
	redFlash = true;
	UpdateHealth(Damage);
	DamageTimer();
}

void AGAM312_FinalCharacter::UpdateHealth(float HealthChange)
{
	Health -= HealthChange;
	Health = FMath::Clamp(Health, 0.f, FullHealth);
	PreviousHealth = HealthPercentage;
	HealthPercentage = (Health/FullHealth);
}

/* Update the ammo and SetAmmoChange */
void AGAM312_FinalCharacter::UpdateAmmo(float AmmoChange)
{
	Ammo += AmmoChange;
	Ammo = FMath::Clamp(Ammo, 0.f, FullAmmo);
	PreviousAmmo = AmmoPercentage;
	AmmoPercentage = (Ammo/FullAmmo);
}

void AGAM312_FinalCharacter::SetAmmoChange(float AmmoChange)
{
	bCanUseAmmo = false; // Temporary delay for firing weapon.
	Ammo += AmmoChange;
	Ammo = FMath::Clamp(Ammo, 0.f, FullAmmo);
	PreviousAmmo = AmmoPercentage;
	AmmoValue += (AmmoChange/FullAmmo);
	if(GunOverheatMaterial)
	{
		FP_Gun->SetMaterial(0, GunOverheatMaterial); // Overheat after usage.
	}
	MyTimeline.PlayFromStart();
}




