// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerEclipseClass.generated.h"

class USoSEclipseMeleeAbility;
class USoSEclipseRangedAbility;
class USoSEclipseOffhandAbility;
class USoSEclipseAuxiliaryAbility;

USTRUCT(BlueprintType)
struct FPlayerEclipseAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseMeleeAbility> MeleeAbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseMeleeAbility> MeleeAbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseMeleeAbility> MeleeAbilityThreeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseRangedAbility> RangedAbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseRangedAbility> RangedAbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseRangedAbility> RangedAbilityThreeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseOffhandAbility> OffhandAbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseOffhandAbility> OffhandAbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseAuxiliaryAbility> AuxAbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseAuxiliaryAbility> AuxAbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSEclipseAuxiliaryAbility> AuxAbilityThreeClass;
};


UCLASS()
class SEAOFSAND_API ASoSPlayerEclipseClass : public ASoSPlayerCharacter
{
	GENERATED_BODY()
	
public:

	ASoSPlayerEclipseClass();
	
protected:

	virtual void BeginPlay() override;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Eclipse")
	void AddValueToEclipseRatio(float Value);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FPlayerEclipseAbilitiesData ClassAbilities;

public:

	UFUNCTION(BlueprintCallable, Category = "Eclipse")
	float GetEclipseRatio() const;

};
