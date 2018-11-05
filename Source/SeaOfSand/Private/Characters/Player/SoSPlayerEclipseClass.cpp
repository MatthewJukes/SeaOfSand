// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerEclipseClass.h"
#include "SoSEclipseAbilityBase.h"
#include "SoSEclipseMeleeAbility.h"
#include "SoSEclipseRangedAbility.h"
#include "SoSEclipseOffhandAbility.h"
#include "SoSEclipseAuxiliaryAbility.h"
#include "SoSCombatComponent.h"
#include "SoSASTasks.h"


ASoSPlayerEclipseClass::ASoSPlayerEclipseClass()
{
	ClassSpecificFloat = 50;
}

void ASoSPlayerEclipseClass::BeginPlay()
{
	Super::BeginPlay();

	AbilityBar.MeleeAbilityOne = USoSASTasks::CreateAbilityInstance(ClassAbilities.MeleeAbilityOneClass, CombatComp);
	AbilityBar.MeleeAbilityTwo = USoSASTasks::CreateAbilityInstance(ClassAbilities.MeleeAbilityTwoClass, CombatComp);
	AbilityBar.MeleeAbilityOne = USoSASTasks::CreateAbilityInstance(ClassAbilities.MeleeAbilityOneClass, CombatComp);

	AbilityBar.RangedAbilityOne = USoSASTasks::CreateAbilityInstance(ClassAbilities.RangedAbilityOneClass, CombatComp);
	AbilityBar.RangedAbilityTwo = USoSASTasks::CreateAbilityInstance(ClassAbilities.RangedAbilityTwoClass, CombatComp);
	AbilityBar.RangedAbilityThree = USoSASTasks::CreateAbilityInstance(ClassAbilities.RangedAbilityThreeClass, CombatComp);

	AbilityBar.OffhandAbilityOne = USoSASTasks::CreateAbilityInstance(ClassAbilities.OffhandAbilityOneClass, CombatComp);
	AbilityBar.OffhandAbilityTwo = USoSASTasks::CreateAbilityInstance(ClassAbilities.OffhandAbilityTwoClass, CombatComp);

	AbilityBar.AuxAbilityOne = USoSASTasks::CreateAbilityInstance(ClassAbilities.AuxAbilityOneClass, CombatComp);
	AbilityBar.AuxAbilityTwo = USoSASTasks::CreateAbilityInstance(ClassAbilities.AuxAbilityTwoClass, CombatComp);
	AbilityBar.AuxAbilityThree = USoSASTasks::CreateAbilityInstance(ClassAbilities.AuxAbilityThreeClass, CombatComp);
}


void ASoSPlayerEclipseClass::AddValueToEclipseRatio(float Value)
{
	ClassSpecificFloat = FMath::Clamp(ClassSpecificFloat + Value, 0.0f, 100.0f);
}


float ASoSPlayerEclipseClass::GetEclipseRatio() const
{
	return ClassSpecificFloat;
}
