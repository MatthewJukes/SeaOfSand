// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SoSGameModeBase.generated.h"

class USoSCombatComponent;

UCLASS()
class SEAOFSAND_API ASoSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASoSGameModeBase();

	virtual void Tick(float DeltaSeconds) override;

public:

	void AddCombatComponentToArray(USoSCombatComponent* CombatComp);

	void RemoveCombatComponentToArray(USoSCombatComponent* CombatComp);

protected:

	TArray<USoSCombatComponent*> CombatComponentsToCheck;	

	void CheckCombatComponents();
	
};
