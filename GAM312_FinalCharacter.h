// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"
#include "GAM312_FinalCharacter.generated.h"


class UInputComponent;

UCLASS(config=Game)
class AGAM312_FinalCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Pawn mesh: 3rd person view (body; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AGAM312_FinalCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Boolean for first person camera enabled or not. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	uint32 FirstPerson : 1;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGAM312_FinalProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float PreviousHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float FullAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float AmmoPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float PreviousAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float AmmoValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool redFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	UCurveFloat *AmmoCurve;

	FTimerHandle MemberTimerHandle;
	FTimerHandle AmmoTimerHandle;
	FTimeline MyTimeline;
	float CurveFloatValue;
	float TimelineValue;

	bool bCanUseAmmo;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	UFUNCTION(BlueprintPure, Category = "Ammo")
	float GetAmmo();

	UFUNCTION(BlueprintPure, Category = "Health")
	FText GetHealthIntText();

	UFUNCTION(BlueprintPure, Category = "Ammo")
	FText GetAmmoIntText();

	UFUNCTION()
	void DamageTimer();

	UFUNCTION()
	void SetDamageState();

	UFUNCTION()
	void SetAmmoValue();

	UFUNCTION()
	void SetAmmoState();

	UFUNCTION()
	void SetAmmoChange(float AmmoChange);

	UFUNCTION()
	void UpdateAmmo(float AmmoChange);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool PlayFlash();

	UPROPERTY(EditAnywhere, Category = "Ammo")
	class UMaterialInterface* GunDefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	class UMaterialInterface* GunOverheatMaterial;

	UFUNCTION() // Event for taking damage, which can be used to pass the damage amount to a blueprint if need be.
	void TakeDamage(AActor *DamagedActor, float Damage, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Power")
	void UpdateHealth(float HealthChange);


private:
	void DisplayRaycast();

protected:
	/** Toggle Camera View*/
	void ToggleCamera();

	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns Mesh3P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh3P; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns ThirdPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetThirdPersonCameraComponent() const { return ThirdPersonCameraComponent; }
};

