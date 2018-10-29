// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerEclipseClass.generated.h"

class USoSEclipseMeleeAbility;
class USoSEclipseRangedAbility;
class USoSEclipseAuxiliaryAbility;

USTRUCT(BlueprintType)
struct FPlayerEclipseAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityThreeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityFourClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityFiveClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySixClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySevenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityEightClass;
};


UCLASS()
class SEAOFSAND_API ASoSPlayerEclipseClass : public ASoSPlayerCharacter
{
	GENERATED_BODY()
	
	
	
	
};
