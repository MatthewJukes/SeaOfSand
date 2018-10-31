// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSEffectData.h"
#include "Components/ActorComponent.h"
#include "SoSCombatComponent.generated.h"


class USoSAbilityBase;
class ASoSCharacterBase;
class ASoSWeaponBase;
enum class EAbilityResourceType : uint8;


UENUM(BlueprintType)
enum class EOwnerState : uint8
{
	Normal,
	Sprinting,
	Aiming,
	PerformingAction,
	Dead
};


UENUM(BlueprintType)
enum class EEffectUpdateEventType : uint8
{
	Added,
	Removed,
	Reapplied
};


UENUM(BlueprintType)
enum class ETagUpdateEventType : uint8
{
	Added,
	Removed
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectUpdate, USoSCombatComponent*, CombatComp, const FEffectData&, Effect, EEffectUpdateEventType, EventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagUpdate, const EAbilityTag&, Tag, ETagUpdateEventType, EventType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSCombatComponent();

protected:

	virtual void BeginPlay() override;

public:

	void LoopOverCurrentEffectsArray();

	void AddEffectToArray(FEffectData& EffectToAdd);

	void AddValueToAttributeBaseValues(EAttributeName Attribute, float Value);

	bool UseAbility(USoSAbilityBase* Ability, bool bReleashed = false, float ClassSpecificFloatValue = 0);

	void DamageCalculation(float Damage, ESoSDamageTypeName DamageTypeName);

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void AbilityActionStart();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void AbilityReadyComboAction();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void AbilityActionComplete();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectUpdate OnEffectUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTagUpdate OnTagUpdate;

protected:

	TArray<FEffectData> CurrentEffects;

	TArray<EAbilityTag> CurrentEffectTags;

	FAttributeData AttributeBaseValues;

	FAttributeData AttributeTotalValues;

	FAttributeData AttributeTempAdditiveValues;

	FAttributeData AttributeTempMultiplierValues;

	EOwnerState OwnerState;

	USoSAbilityBase* LastAbilityToStartMontage;

	ASoSCharacterBase* OwningCharacter;

	FName WeaponProjectileOriginSocketName;

	FVector* AimHitLocation;

	UDataTable* DamageTypeDataTable;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	ESoSTeam Team;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float HealthMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float ArmourMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float EnergyMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float SpeedStartValue;

	void CheckEffectStatus(FEffectData& Effect);

	void HandleEffectAttributeModifierValue(FEffectData& Effect, FEffectAttributeModifierModule& Module, bool bUseTotalValue);

	void HandleEffectAbility(FEffectData& Effect, FEffectAbilityModule& Module);

	UFUNCTION()
	void TagUpdate(const EAbilityTag& Tag, ETagUpdateEventType EventType);

	void AddValueToAttributeData(FAttributeData& AttributeData, EAttributeName Attribute, float Value);

	void MultiplyAttributeDataByValue(FAttributeData& AttributeData, EAttributeName Attribute, float Value);

	void CalculateAttributeTotalValues();

	void RemoveEffectFromArrayByIndex(int32 Index);

	void RemoveEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove);

	void EndEffect(FEffectData& EffectToEnd);

	bool AbilityCheckCooldownAndCharges(USoSAbilityBase* AbilityToCheck, bool bReleashed);

	bool AbilityHandleResource(EAbilityResourceType Type, float Cost, bool bReleashed);

////////////////////////////////////////////////
// Getters and Setters

public:
	
	float GetAttributeDataValue(FAttributeData* AttributeData, EAttributeName AttributeToGet) const;

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	float GetAttributeTotalValue(EAttributeName AttributeToGet) const;

	TArray<FEffectData>& GetCurrentEffectsArray();

	TArray<EAbilityTag>& GetCurrentEffectTags();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	EOwnerState GetOwnerState() const;

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	USoSAbilityBase* GetLastAbilityToStartMontage() const;

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	ASoSCharacterBase* GetOwningCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	ESoSTeam GetTeam() const;

	FVector* GetAimHitLocation() const;

	void SetOwnerState(EOwnerState NewState);

	void SetWeaponProjectileOriginSocketName(FName SocketName);

	void SetAimHitLocation(FVector* AimHitLocationReference);

	void SetLastAbilityToStartMontage(USoSAbilityBase* Ability);
};
