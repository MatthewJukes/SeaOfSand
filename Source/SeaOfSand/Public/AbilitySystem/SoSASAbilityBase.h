// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASAbilityBase.generated.h"

UENUM()
enum class EASResourceType : uint8
{
	Energy,
	Health
};

UCLASS(BlueprintType, Blueprintable)
class SEAOFSAND_API USoSASAbilityBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	bool StartAbility(AActor* CurrentTarget, AActor* Instigator, USkeletalMeshComponent* WeaponMesh, FName SocketName, float TimeSeconds, UWorld* World);

private:

	float LastTimeActivated;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Cost;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EASResourceType ResourceType;

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetLastTimeActivated() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCost() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	EASResourceType GetResourceType() const;

	void SetLastTimeActivated(float NewTime);
};