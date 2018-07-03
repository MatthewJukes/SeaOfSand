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

	ASoSPlayerController* PlayerController;

	ASoSPlayerCharacter* PlayerCharacter;

	int32 CurrentAmmo;

	int32 CurrentAmmoInClip;

	EWeaponState CurrentWeaponState;

	EWeaponType WeaponType;

	bool bGettingAccuracyBonus;

	bool bCanReload;

	float TimeBetweenShots;

	float LastFireTime;

	float AimingStartTime;

	float CurrentRecoil;

	/* RPM - Bullet per minute fire by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Firing")
	int32 ProjectilesPerShot;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float VulnerableHitBonusDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float WeaponDrawnSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Movement")
	float AimingSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	FVector2D BaseBulletSpreadRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	FVector2D AimingBulletSpreadRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	float RecoilAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	float RecoilRecoveryTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Accuracy")
	float AimingBulletSpreadLerpTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 StartAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	float ReloadDuration;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon | Names")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon | Names")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Effects")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Effects")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Effects")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Effects")
	UParticleSystem* TracerEffect;

	void HandleFiring();

	bool CheckIfWeaponCanFire();

	void UseAmmo();

	void UpdateRecoil();
	
	void ReloadWeapon();

	void PlayMuzzleEffect();

	void PlayTracerEffect(FVector TraceEnd);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	bool WeaponTrace(FHitResult& OutHitlocation, FVector StartLocation, FVector EndLocation) const;

	FVector GetAimDirection();	

	/* Timer handles */
	FTimerHandle TimerHandle_TimerBetweenShots;
	FTimerHandle TimerHandle_ReloadTime;
	FTimerHandle TimerHandle_ReduceRecoil;

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

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetBulletSpread() const;
};
