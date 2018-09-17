// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASAbilityBase.generated.h"


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

public:

	float GetLastTimeActivated() const;

	float GetCooldown();

	void SetLastTimeActivated(float NewTime);
};