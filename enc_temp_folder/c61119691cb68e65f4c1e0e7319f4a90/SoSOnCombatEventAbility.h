// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/SoSAbilityBase.h"
#include "SoSOnCombatEventAbility.generated.h"


class USoSCombatComponent;
enum class EEffectCombatEventTriggerType : uint8;


UCLASS()
class SEAOFSAND_API USoSOnCombatEventAbility : public USoSAbilityBase
{
	GENERATED_BODY()
	
public:

	void BindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType);

	void UnbindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType);

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEventAbility")
	void OnDamageDealt(USoSCombatComponent* TargetCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEventAbility")
	void OnDamageReceived(USoSCombatComponent* SourceCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEventAbility")
	void OnEffectUpdate(const FEffectData& Effect, EEffectUpdateEventType EventType);
};
