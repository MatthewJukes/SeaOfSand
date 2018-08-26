// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffectData.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"

class USoSASAbilityBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSASComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void AddASEffectToArray(FASEffectData* EffectToAdd);

	void RemoveASEffectFromArray(FASEffectData* EffectToRemove);

	void EndASEffect(FASEffectData* EffectToEnd);

	void UseAbility(USoSASAbilityBase* AbilityToUse);

protected:

	TArray<FASEffectData*> CurrentPositiveEffects;

	TArray<FASEffectData*> CurrentNeutralEffects;

	TArray<FASEffectData*> CurrentNegativeEffects;

	TArray<FASEffectData*> DefaultArray; // Empty array for switch statements

	FASAttributeData ASAttributeBaseValues;

	FASAttributeData ASAttributeTotalValues;

	FASAttributeData ASAttributeTempAdditiveValues;

	FASAttributeData ASAttributeTempMultiplierValues; // Both Multiplicative and Subtractive values

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float HealthMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float ArmourMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float EnergyMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float SpeedStartValue;
	
	void LoopOverCurrentASEffectsArrays();

	void CheckASEffectStatus(FASEffectData* Effect);

	void HandleASEffectValue(FASEffectData* Effect, bool bUseTotalValue);

	void AddValueToASAttributeData(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void MultiplyASAttributeDataByValue(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void CalculateASAttributeTotalValues();

////////////////////////////////////////////////
// Getters and Setters

public:

	float GetASAttribute(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const;

	TArray<FASEffectData*>& GetCurrentEffectsArray(EASEffectType EffectType);

};
