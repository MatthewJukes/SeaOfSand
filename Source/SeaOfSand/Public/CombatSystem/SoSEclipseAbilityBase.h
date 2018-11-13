// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/SoSAbilityBase.h"
#include "SoSEclipseAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class SEAOFSAND_API USoSEclipseAbilityBase : public USoSAbilityBase
{
	GENERATED_BODY()
	
protected:

	USoSEclipseAbilityBase();
	
protected:

	int32 SunPoints;

	int32 MoonPoints;

	virtual bool StartAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue) override;

	virtual bool ReleaseAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue) override;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Eclipse")
	int32 GetSunPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Eclipse")
	int32 GetMoonPoints() const;
};
