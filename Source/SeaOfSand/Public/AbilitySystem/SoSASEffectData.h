// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoSASEffectData.generated.h"

enum class EASAttributeName : uint8;
enum class EASTag : uint8;

UENUM(BlueprintType)
enum class EASEffectType : uint8
{
	AttributeModifier,
	DamageDealer
};


UENUM(BlueprintType)
enum class EASEffectAlignment : uint8
{
	Positive,
	Neutral,
	Negative
};


UENUM(BlueprintType)
enum class EASEffectValueType : uint8 // Attribute modification formula type
{
	Additive,
	Multiplicative,
	Subtractive
};


USTRUCT(BlueprintType)
struct FASEffectData : public FTableRowBase
{
	GENERATED_BODY()

	// Effect properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	FName EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	FName EffectDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UTexture2D* EffectIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bHideFromUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectType EffectType;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AttributeModifier")
	EASAttributeName AttributeToEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageDealer")
	FDataTableRowHandle DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectAlignment EffectAlignment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectValueType EffectValueType;

	// Magnitude of the effect. Multiplicative and Subtractive should be given in percentage values. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float EffectValue;

	// The rate in seconds at which the value is reapplied over the duration of the effect, 0 means the effect will tick only once
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float TickRate; 

	// Delay the first value application of the effect, e.g. an effect that ticks once will have the value applied when the duration has ended instead of at the start.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bDelayFirstTick; 

	// Maximum number of times the effect can be applied to the same target at the same time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "1"))
	int32 MaxStacks = 1;

	// Is the effect a temporary stat modifier? Generally should be true when effect things like max health or speed and you want these to be restored when the effect end. False for an effect like draining current health or energy.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bTemporaryModifier; 

	// If reapplied, duration added to remaining duration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bAdditiveDuration;

	// Tags to apply to target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect | Tags")
	TArray<EASTag> EffectAppliesTags;

	// Tags that will stop the effect from being applied or remove it if in effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect | Tags")
	TArray<EASTag> EffectBlockedByTags;

	// Tags that will cause the effect to have no effect, but will not stop it's application or remove it or it's tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect | Tags")
	TArray<EASTag> EffectNegatedByTags;

	// Tags required for the effect to to be applied, effect will not be removed if the tag is removed/expires
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect | Tags")
	TArray<EASTag> EffectRequiresTags;

	// Effect status trackers

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	float EffectDuration; 

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	float EffectStartTime;

	float LastTickTime;

	float TotalValue;

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	int32 CurrentStacks;

	int32 NewStacks;

	bool bNonTicking;

	bool bExpired = false;

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const;
#endif
};