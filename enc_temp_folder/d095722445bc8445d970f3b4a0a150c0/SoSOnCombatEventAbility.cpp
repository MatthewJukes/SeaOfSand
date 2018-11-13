// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSOnCombatEventAbility.h"
#include "SoSCombatComponent.h"
#include "SoSEffectData.h"


void USoSOnCombatEventAbility::BindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType)
{
	switch (EventType)
	{
	case EEffectCombatEventTriggerType::OnDamageDealt:
		CombatComp->OnDamageDealt.AddDynamic(this, &USoSOnCombatEventAbility::OnDamageDealt);
		break;
	case EEffectCombatEventTriggerType::OnDamageReceived:
		CombatComp->OnDamageReceived.AddDynamic(this, &USoSOnCombatEventAbility::OnDamageReceived);
		break;
	case EEffectCombatEventTriggerType::OnEffectUpdate:
		CombatComp->OnEffectUpdate.AddDynamic(this, &USoSOnCombatEventAbility::OnEffectUpdate);
		break;
	default:
		break;
	}
}

void USoSOnCombatEventAbility::UnbindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType)
{
	switch (EventType)
	{
	case EEffectCombatEventTriggerType::OnDamageDealt:
		CombatComp->OnDamageDealt.RemoveAll(this);
		break;
	case EEffectCombatEventTriggerType::OnDamageReceived:
		CombatComp->OnDamageReceived.RemoveAll(this);
		break;
	case EEffectCombatEventTriggerType::OnEffectUpdate:
		CombatComp->OnEffectUpdate.RemoveAll(this);
		break;
	default:
		break;
	}
}
