// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class ABasePlayerController;

UCLASS()
class SEAOFSANDS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()	
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartFiring();
	virtual void StopFiring();

protected:

	bool CheckIfWeaponCanFire(float FireRate) const;
	bool WeaponTrace(FVector& OutHitlocation, FVector MuzzleLocation, float MaxRange, float BulletSpread) const;

	ABasePlayerController* PlayerController;

private:

	FVector GetAimDirection(FVector StartLocation, float BulletSpread) const;
};
