// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffect.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"


USTRUCT(BlueprintType)
struct FASCharacterData
{
	GENERATED_USTRUCT_BODY()

	float HealthCurrentValue;

	float ArmourCurrentValue;

	float SpeedCurrentValue;

	float EnergyCurrentValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float HealthMaxValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float ArmourMaxValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float SpeedBaseValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float EnergyMaxValue;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSASComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);

protected:

	virtual void BeginPlay() override;

public:

	void AddASEffectToArray(FASEffectData* EffectToAdd);

	void RemoveASEffectFromArray(FASEffectData* EffectToRemove);

	void EndASEffect(FASEffectData* EffectToEnd);

protected:

	TArray<FASEffectData*> CurrentPositiveEffects;

	TArray<FASEffectData*> CurrentNeutralEffects;

	TArray<FASEffectData*> CurrentNegativeEffects;

	TArray<FASEffectData*> DefaultArray;

	TArray<float> ASAttributeTempAdditiveValues;

	TArray<float> ASAttributeTempMultiplierValues; // Both Multiplicative and Subtractive values
	
	void LoopOverCurrentASEffectsArrays();

	void CheckASEffectStatus(FASEffectData* Effect);

	void CheckASEffectValue(FASEffectData* Effect);

	void AddValueToASAttributeTempAdditiveValues(EASAttributeName Attribute, float Value);

	void AddValueToASAttributeTempMultiplierValues(EASAttributeName Attribute, float Value);

	void AddValueToASAttribute(EASAttributeName Attribute, float Value);

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	FASCharacterData* CharacterData;

////////////////////////////////////////////////
// Getters and Setters

public:

	float GetASAttribute(EASAttributeName AttributeToGet) const;

	TArray<FASEffectData*>& GetCurrentEffectsArray(EASEffectType EffectType);

	void SetASAttribute(EASAttributeName AttributeToSet, float Value);
};
