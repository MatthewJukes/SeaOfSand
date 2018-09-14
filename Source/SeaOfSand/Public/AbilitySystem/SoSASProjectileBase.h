// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoSASProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SEAOFSAND_API ASoSASProjectileBase : public AActor
{
	GENERATED_BODY()

public:

	ASoSASProjectileBase();

protected:

	//virtual void

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UProjectileMovementComponent* ProjectileMovement;

public:

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float BaseDamage;

};
