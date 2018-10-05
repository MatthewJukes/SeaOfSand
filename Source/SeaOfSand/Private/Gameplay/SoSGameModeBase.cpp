// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSGameModeBase.h"
#include "SoSASComponent.h"


ASoSGameModeBase::ASoSGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ASoSGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckASComponents();
}


void ASoSGameModeBase::AddASComponentToArray(USoSASComponent* ASComp)
{
	UE_LOG(LogTemp, Warning, TEXT("ASComp Added"))
	ASComponentsToCheck.Add(ASComp);
}


void ASoSGameModeBase::RemoveASComponentToArray(USoSASComponent* ASComp)
{
	ASComponentsToCheck.RemoveSwap(ASComp);
}


void ASoSGameModeBase::CheckASComponents()
{
	for (USoSASComponent* ASComp : ASComponentsToCheck)
	{
		ASComp->LoopOverCurrentASEffectsArray();
	}
}
