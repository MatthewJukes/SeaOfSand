// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/SoSBasicAttackAbility.h"
#include "SoSBasicAttackAbilityMelee.generated.h"


class ASoSMeleeWeapon;


UCLASS()
class SEAOFSAND_API USoSBasicAttackAbilityMelee : public USoSBasicAttackAbility
{
	GENERATED_BODY()
	
	
public:

	virtual void InitializeAbility_Implementation() override;

	virtual void HitboxActivate_Implementation() override;

	virtual void HitboxDeactivate_Implementation() override;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Ability | BasicAttackMelee")
	ASoSMeleeWeapon* MeleeWeapon;
	
	UPROPERTY(BlueprintReadWrite, Category = "Ability | BasicAttackMelee")
	TArray<AActor*> IgnoreTargets;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | BasicAttackMelee")
	void MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
