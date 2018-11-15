// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBasicAttackAbility.h"
#include "SoSCombatComponent.h"


bool USoSBasicAttackAbility::ReleaseAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue)
{
	GetOwningCombatComp()->OnBasicAttackExecuted.Broadcast();
	return true;
}
