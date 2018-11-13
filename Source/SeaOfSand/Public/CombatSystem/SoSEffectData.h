// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoSEffectData.generated.h"

class USoSAbilityBase;
class USoSOnCombatEventAbility;
enum class EAttributeName : uint8;
enum class ESoSDamageTypeName : uint8;
enum class EAbilityTag : uint8;


UENUM(BlueprintType)
enum class EEffectAlignment : uint8
{
	Positive,
	Neutral,
	Negative
};


UENUM(BlueprintType)
enum class EEffectValueType : uint8 // Attribute modification formula type
{
	Additive,
	Multiplicative,
	Subtractive
};


UENUM(BlueprintType)
enum class EEffectAbilityTickType : uint8
{
	EveryTick,
	FirstTick,
	LastTick
};


UENUM(BlueprintType)
enum class EEffectCombatEventTriggerType : uint8
{
	OnDamageDealt,
	OnDamageReceived,
	OnEffectUpdate
};


USTRUCT(BlueprintType)
struct FEffectAttributeModifierModule
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EAttributeName AttributeToEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EEffectValueType ModifierValueType;

	// Magnitude of the effect. Multiplicative and Subtractive should be given in percentage values. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float ModifierValue;

	// Is the effect a temporary stat modifier? Generally should be true when effect things like max health or speed and you want these to be restored when the effect end. False for an effect like draining current health or energy.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bTemporaryModifier;

	float TotalValue;
};


USTRUCT(BlueprintType)
struct FEffectDamageModule
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	ESoSDamageTypeName DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.001", UIMin = "0.001"))
	float DamageValue;
};


USTRUCT(BlueprintType)
struct FEffectOnTickAbilityModule
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<USoSAbilityBase> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EEffectAbilityTickType AbilityTickType;

	UPROPERTY()
	USoSAbilityBase* Ability;
};


USTRUCT(BlueprintType)
struct FEffectOnCombatEventAbilityModule
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<USoSOnCombatEventAbility> OnCombatEventAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EEffectCombatEventTriggerType AbilityTriggerType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	int32 MaxTriggers;

	UPROPERTY()
	USoSOnCombatEventAbility* OnCombatEventAbility;
};


USTRUCT(BlueprintType)
struct FEffectData : public FTableRowBase
{
	GENERATED_BODY()

	// Effect properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FName EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FName EffectDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	UTexture2D* EffectIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	bool bHideFromUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	EEffectAlignment EffectAlignment;

	// The rate in seconds at which the value is reapplied over the duration of the effect, 0 means the effect will tick only once
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	float TickRate; 

	// Delay the first value application of the effect, e.g. an effect that ticks once will have the value applied when the duration has ended instead of at the start.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	bool bDelayFirstTick; 

	// Maximum number of times the effect can be applied to the same target at the same time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters", meta = (ClampMin = "1"))
	int32 MaxStacks = 1;

	// If reapplied, duration added to remaining duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	bool bAdditiveDuration;

	// Max duration for additive duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters", meta = (EditCondition = "bAdditiveDuration"))
	float MaxDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modules")
	TArray<FEffectAttributeModifierModule> AttributeModifierModules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modules")
	TArray<FEffectDamageModule> DamageModules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modules")
	TArray<FEffectOnTickAbilityModule> OnTickAbilityModules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modules")
	TArray<FEffectOnCombatEventAbilityModule> OnEventAbilityModules;

	// Tags to apply to target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	TArray<EAbilityTag> EffectAppliesTags;

	// Tags that will stop the effect from being applied or remove it if in effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	TArray<EAbilityTag> EffectBlockedByTags;

	// Tags required for the effect to to be applied, effect will not be removed if the tag is removed/expires
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	TArray<EAbilityTag> EffectRequiresTags;

	// Effect status trackers

	USoSCombatComponent* SourceCombatComp;

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	float EffectDuration; 

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	float EffectStartTime;

	float LastTickTime;

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	int32 CurrentStacks;

	int32 NewStacks;

	bool bNonTicking;

	bool bExpired = false;

	bool bFirstTick = true;
};