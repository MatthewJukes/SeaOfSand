// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSOnCombatEventAbility.h"
#include "SoSCombatComponent.h"
#include "SoSEffectData.h"


void USoSOnCombatEventAbility::BindCombatEvent(USoSCombatComponent* CombatComp, EEffectCombatEventTriggerType EventType)
{
	OwningCombatComp = CombatComp;
	
	switch (EventType)
	{
	case EEffectCombatEventTriggerType::OnDamageDealt:
		CombatComp->OnDamageDealt.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnDamageDealt);
		break;
	case EEffectCombatEventTriggerType::OnDamageReceived:
		CombatComp->OnDamageReceived.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnDamageReceived);
		break;
	case EEffectCombatEventTriggerType::OnEffectUpdate:
		CombatComp->OnEffectUpdate.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnEffectUpdate);
		break;
	case EEffectCombatEventTriggerType::OnBasicAttackExecuted:
		CombatComp->OnBasicAttackExecuted.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnBasicAttackExecuted);
		CombatComp->OnBasicAttackHit.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnBasicAttackHit);
		break;
	case EEffectCombatEventTriggerType::OnBasicAttackRecieved:
		CombatComp->OnBasicAttackReceived.AddDynamic(this, &USoSOnCombatEventAbility::HandleOnBasicAttackRecieved);
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
	case EEffectCombatEventTriggerType::OnBasicAttackExecuted:
		CombatComp->OnBasicAttackExecuted.RemoveAll(this);
		CombatComp->OnBasicAttackHit.RemoveAll(this);
		break;
	case EEffectCombatEventTriggerType::OnBasicAttackRecieved:
		CombatComp->OnBasicAttackReceived.RemoveAll(this);
		break;
	default:
		break;
	}
}


void USoSOnCombatEventAbility::HandleOnDamageDealt(const USoSCombatComponent* TargetCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage)
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	RemainingTriggers--;
	OnDamageDealt(TargetCombatComp, BaseDamage, HealthDamage, ArmourDamage);
}


void USoSOnCombatEventAbility::HandleOnDamageReceived(const USoSCombatComponent* SourceCombatComp, float BaseDamage, float HealthDamage, float ArmourDamage)
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	RemainingTriggers--;
	OnDamageReceived(SourceCombatComp, BaseDamage, HealthDamage, ArmourDamage);
}


void USoSOnCombatEventAbility::HandleOnEffectUpdate(const FEffectData& Effect, EEffectUpdateEventType EventType)
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	RemainingTriggers--;
	OnEffectUpdate(Effect, EventType);
}


void USoSOnCombatEventAbility::HandleOnBasicAttackExecuted()
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	RemainingTriggers--;
	OnBasicAttackExecuted();
}


void USoSOnCombatEventAbility::HandleOnBasicAttackHit(const USoSCombatComponent* TargetCombatComp, ETeamCheckResult TeamCheckResult)
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	OnBasicAttackHit(TargetCombatComp, TeamCheckResult);
}


void USoSOnCombatEventAbility::HandleOnBasicAttackRecieved(const USoSCombatComponent* SourceCombatComp, ETeamCheckResult TeamCheckResult)
{
	if (RemainingTriggers == 0)
	{
		return;
	}

	RemainingTriggers--;
	OnBasicAttackRecieved(SourceCombatComp, TeamCheckResult);
}


int32 USoSOnCombatEventAbility::GetRemainingTriggers()
{
	return RemainingTriggers;
}


void USoSOnCombatEventAbility::SetRemainingTriggers(int32 Value)
{
	RemainingTriggers = Value;
}
