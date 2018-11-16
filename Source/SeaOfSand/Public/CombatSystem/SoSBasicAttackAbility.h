// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/SoSAbilityBase.h"
#include "SoSBasicAttackAbility.generated.h"


UCLASS()
class SEAOFSAND_API USoSBasicAttackAbility : public USoSAbilityBase
{
	GENERATED_BODY()
	
protected:

	bool bOnBasicAttackTriggered;

	virtual bool StartAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue) override;

	virtual bool ReleaseAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue) override;
	
public:

};
