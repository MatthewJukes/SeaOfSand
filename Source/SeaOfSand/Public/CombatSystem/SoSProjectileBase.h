// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSAbilityActor.h"
#include "SoSProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SEAOFSAND_API ASoSProjectileBase : public ASoSAbilityActor
{
	GENERATED_BODY()

public:

	ASoSProjectileBase();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UProjectileMovementComponent* ProjectileMovement;

public:

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float ProjectileDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float ProjectileSpeed;

public:

	void SetProjectileDamage(float Value);

	void SetProjectileSpeed(float Value);
};
