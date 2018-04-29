// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class ASoSPlayerController;
class APlayerCharacter;
class ABaseProjectile;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Equipping,
	Reloading,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol,
	Rifle,
	Shotgun,
};

UCLASS()
class SEAOFSAND_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* ShotAudioComponent;
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void StartFiring();
	void StopFiring();
	void StartReload();
	void InterruptReload();

	// Weapon is getting bonus accuracy from player aiming
	bool bAimingBonus;

	bool bCanReload;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	EWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float WeaponDrawnSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float AimingSpeedMultiplier;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	float BaseBulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	float AimingSpreadMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 StartAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	float ReloadDuration;

	// Variables exposed to BP for UI 
	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmoInClip;

private:	

	void HandleFiring();

	// Check if weapon/owner can fire the weapon
	bool CheckIfWeaponCanFire(float FireRate);

	// Calculate the amount the bullets will deviate from the aim location
	float CalculateBulletSpread();

	void UseAmmo();
	
	void ReloadWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	TSubclassOf<ABaseProjectile> ProjectileBlueprint;

	void FireProjectile(FVector AimDirection);

	// Weapon trace
	bool WeaponTrace(FVector& OutHitlocation, FVector AimDirection) const;
	FVector GetAimDirection() const;	

	// Controller and Player references
	ASoSPlayerController* PlayerController;
	APlayerCharacter* PlayerCharacter;

	// Timer handles
	FTimerHandle FireRateTimerHandle;
	FTimerHandle RefireTimerHandle;
	FTimerHandle ReloadTimerHandle;
};
