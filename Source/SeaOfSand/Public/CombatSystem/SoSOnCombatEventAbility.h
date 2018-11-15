// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/SoSAbilityBase.h"
#include "SoSOnCombatEventAbility.generated.h"


class USoSCombatComponent;
enum class EEffectCombatEventTriggerType : uint8;
enum class ETeamCheckResult : uint8;


UCLASS()
class SEAOFSAND_API USoSOnCombatEventAbility : public USoSAbilityBase
{
	GENERATED_BODY()
	
public:

	void BindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType);

	void UnbindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType);

protected:	

	int32 RemainingTriggers;

	UFUNCTION()
	void HandleOnDamageDealt(const USoSCombatComponent* TargetCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION()
	void HandleOnDamageReceived(const USoSCombatComponent* SourceCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION()
	void HandleOnEffectUpdate(const FEffectData& Effect, EEffectUpdateEventType EventType);

	UFUNCTION()
	void HandleOnBasicAttackExecuted();

	UFUNCTION()
	void HandleOnBasicAttackHit(const USoSCombatComponent* TargetCombatComp, ETeamCheckResult TeamCheckResult);

	UFUNCTION()
	void HandleOnBasicAttackRecieved(const USoSCombatComponent* SourceCombatComp, ETeamCheckResult TeamCheckResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnDamageDealt(const USoSCombatComponent* TargetCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnDamageReceived(const USoSCombatComponent* SourceCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnEffectUpdate(const FEffectData& Effect, EEffectUpdateEventType EventType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnBasicAttackExecuted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnBasicAttackHit(const USoSCombatComponent* TargetCombatComp, ETeamCheckResult TeamCheckResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | CombatEvent")
	void OnBasicAttackRecieved(const USoSCombatComponent* SourceCombatComp, ETeamCheckResult TeamCheckResult);

public:

	int32 GetRemainingTriggers();

	void SetRemainingTriggers(int32 Value);

};
