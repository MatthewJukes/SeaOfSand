// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoSWeaponBase.generated.h"

class ASoSPlayerController;
class ASoSPlayerCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Ranged,
	Melee
};

UCLASS()
class SEAOFSAND_API ASoSWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USkeletalMeshComponent* WeaponMesh;

public:	

protected:

	ASoSPlayerController* PlayerController;

	ASoSPlayerCharacter* PlayerCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float VulnerableHitBonusDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	TSubclassOf<UDamageType> DamageType;

public:

	/* Getters and Setters */

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const;
};
