// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoSWeaponBase.generated.h"

class ASoSPlayerController;
class ASoSPlayerCharacter;
class USoSAbilityBase;
	
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Ranged,
	Melee
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Holstered,
	Equipping,
	Idle,
	Attacking,
	Reloading
};

USTRUCT(BlueprintType)
struct FWeaponAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USoSAbilityBase> AbilityWeaponDrawClass;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USoSAbilityBase> AbilityWeaponHolsterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USoSAbilityBase> AbilityWeaponPrimaryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USoSAbilityBase> AbilityWeaponAltClass;

	UPROPERTY()
	USoSAbilityBase* AbilityWeaponDraw;

	UPROPERTY()
	USoSAbilityBase* AbilityWeaponHolster;

	UPROPERTY()
	USoSAbilityBase* AbilityWeaponPrimary;

	UPROPERTY()
	USoSAbilityBase* AbilityWeaponAlt;
};

UCLASS()
class SEAOFSAND_API ASoSWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASoSWeaponBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USkeletalMeshComponent* WeaponMesh;

public:	

	virtual void StartAttack();

	virtual void EndAttack();

protected:

	ASoSPlayerController* PlayerController;

	ASoSPlayerCharacter* PlayerCharacter;

	EWeaponState WeaponState;
	
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Abilities")
	FWeaponAbilitiesData WeaponAbilities;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon | Names")
	FName ProjectileOriginSocketName;

public:

	/* Getters and Setters */

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponState GetWeaponState() const;

	USkeletalMeshComponent* GetWeaponMesh() const;

	FWeaponAbilitiesData& GetWeaponAbilities();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FVector GetProjectileOriginLocation() const;

	void SetWeaponState(EWeaponState NewState);
};
