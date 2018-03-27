// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class SEAOFSANDS_API ARifle : public ABaseWeapon
{
	GENERATED_BODY()

	//USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* ShotAudioComponent;

	//UPROPERTY(VisibleAnywhere)
	//UParticleSystemComponent* Beam;

public:
	// Sets default values for this actor's properties
	ARifle();

	virtual void StartFiring() override;
	virtual void StopFiring() override;

protected:

private:

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float FireRate = 0.075;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float MaxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float BaseBulletSpread = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float AimingSpreadMultiplier = .35f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	bool bIsAutomatic = true;

	//UPROPERTY(EditDefaultsOnly, Category = "Particles")
	//TSubclassOf<UParticleSystemComponent> Beam;

	void FireBullet();

	FTimerHandle FireRateTimerHandle;
	FTimerHandle RefireTimerHandle;
};
