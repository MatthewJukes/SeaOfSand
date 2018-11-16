// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBasicAttackAbility.h"
#include "SoSCombatComponent.h"


bool USoSBasicAttackAbility::StartAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue)
{
	bOnBasicAttackTriggered = true;
	GetOwningCombatComp()->OnBasicAttackExecuted.Broadcast();
	return true;
}

bool USoSBasicAttackAbility::ReleaseAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue)
{
	if (!bOnBasicAttackTriggered)
	{
		GetOwningCombatComp()->OnBasicAttackExecuted.Broadcast();
	}
	bOnBasicAttackTriggered = false;
	return true;
}
