// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSGameModeBase.h"
#include "SoSCombatComponent.h"


ASoSGameModeBase::ASoSGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ASoSGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckCombatComponents();
}


void ASoSGameModeBase::AddCombatComponentToArray(USoSCombatComponent* CombatComp)
{
	UE_LOG(LogTemp, Warning, TEXT("CombatComp Added"))
	CombatComponentsToCheck.Add(CombatComp);
}


void ASoSGameModeBase::RemoveCombatComponentToArray(USoSCombatComponent* CombatComp)
{
	CombatComponentsToCheck.RemoveSwap(CombatComp);
}


void ASoSGameModeBase::CheckCombatComponents()
{
	for (USoSCombatComponent* CombatComp : CombatComponentsToCheck)
	{
		CombatComp->LoopOverCurrentEffectsArray();
	}
}
