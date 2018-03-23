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

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Rifle;

public:
	// Sets default values for this actor's properties
	ARifle();

	virtual void StartFiring() override;
	virtual void StopFiring() override;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float FireRate = 0.05;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float MaxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float BulletSpread = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	bool bIsAutomatic = true;

	void FireBullet();
	
	FTimerHandle FireRateTimerHandle;
};
