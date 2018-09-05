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

	void AddASEffectToArray(FASEffectData& EffectToAdd);

	void RemoveASEffectFromArrayByIndex(int32 Index);

	void RemoveASEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove);

	void EndASEffect(FASEffectData& EffectToEnd);

	void UseAbility(USoSASAbilityBase* Ability);

protected:

	TArray<FASEffectData> CurrentEffects;

	TArray<EASTag> CurrentTags;

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
	
	void LoopOverCurrentASEffectsArray();

	void CheckASEffectStatus(FASEffectData& Effect);

	void HandleASEffectValue(FASEffectData& Effect, bool bUseTotalValue);

	void AddValueToASAttributeData(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void MultiplyASAttributeDataByValue(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void CalculateASAttributeTotalValues();

////////////////////////////////////////////////
// Getters and Setters

public:

	
	float GetASAttributeDataValue(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetASAttributeTotalValue(EASAttributeName AttributeToGet) const;

	TArray<FASEffectData>& GetCurrentEffectsArray();

};
