// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffectData.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"

class USoSASAbilityBase;
class ASoSGameModeBase;
class ACharacter;

UENUM(BlueprintType)
enum class EASOwnerState : uint8
{
	Normal,
	Sprinting,
	Aiming,
	Attacking,
	Dashing,
	Dead
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSASComponent();

protected:

	virtual void BeginPlay() override;

public:

	void LoopOverCurrentASEffectsArray();

	void AddASEffectToArray(FASEffectData& EffectToAdd);

	bool UseAbility(USoSASAbilityBase* Ability);

protected:

	TArray<FASEffectData> CurrentASEffects;

	TArray<EASTag> CurrentASEffectTags;

	FASAttributeData ASAttributeBaseValues;

	FASAttributeData ASAttributeTotalValues;

	FASAttributeData ASAttributeTempAdditiveValues;

	FASAttributeData ASAttributeTempMultiplierValues;

	EASOwnerState OwnerState;

	ASoSGameModeBase* SoSGameMode;

	ACharacter* ComponentOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float HealthMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float ArmourMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float EnergyMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float SpeedStartValue;

	void CheckASEffectStatus(FASEffectData& Effect);

	void HandleASEffectValue(FASEffectData& Effect, bool bUseTotalValue);

	void AddValueToASAttributeData(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void MultiplyASAttributeDataByValue(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value);

	void CalculateASAttributeTotalValues();

	void RemoveASEffectFromArrayByIndex(int32 Index);

	void RemoveASEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove);

	void EndASEffect(FASEffectData& EffectToEnd);

////////////////////////////////////////////////
// Getters and Setters

public:
	
	float GetASAttributeDataValue(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	float GetASAttributeTotalValue(EASAttributeName AttributeToGet) const;

	TArray<FASEffectData>& GetCurrentEffectsArray();

	TArray<EASTag>& GetCurrentASEffectTags();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	EASOwnerState GetASOwnerState() const;

	void SetASOwnerState(EASOwnerState NewState);
};
