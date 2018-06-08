// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoSBaseWeapon.generated.h"

class ASoSPlayerController;
class ASoSPlayerCharacter;
class ABaseProjectile;
class UAudioComponent;

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
class SEAOFSAND_API ASoSBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSBaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UAudioComponent* ShotAudioComponent;

public:	

	void StartFiring();

	void StopFiring();

	void StartReload();

	void InterruptReload();

protected:

	ASoSPlayerController * PlayerController;

	ASoSPlayerCharacter* PlayerCharacter;

	int32 CurrentAmmo;

	int32 CurrentAmmoInClip;

	EWeaponState CurrentWeaponState;

	EWeaponType WeaponType;

	bool bGettingAccuracyBonus;

	bool bCanReload;

	float TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullet per minute fire by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	int32 ProjectilesPerShot;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float WeaponDrawnSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float AimingSpeedMultiplier;

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

	void HandleFiring();

	bool CheckIfWeaponCanFire();

	float CalculateBulletSpread();

	void UseAmmo();
	
	void ReloadWeapon();

	bool WeaponTrace(FVector& OutHitlocation, FVector AimDirection) const;

	FVector GetAimDirection() const;	

	/* Timer handles */
	FTimerHandle TimerHandle_TimerBetweenShots;
	FTimerHandle TimerHandle_ReloadTime;

public:

	/* Getters and Setters */

	void SetWeaponState(EWeaponState NewState);

	float GetWeaponDrawnSpeedMultiplier() const;

	float GetAimingSpeedMultiplier() const;

	void SetGettingAccuracyBonus(bool bGettingBonus);

	void SetCanReload(bool bReload);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponState GetCurrentWeaponState() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmoInClip() const;
};
