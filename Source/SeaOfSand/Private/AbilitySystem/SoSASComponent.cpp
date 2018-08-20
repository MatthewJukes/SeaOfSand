// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{
	ASAttributeBaseValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));
	ASAttributeTotalValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));
	ASAttributeTempMultiplierValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));
	ASAttributeTempAdditiveValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));

	HealthMaxStartValue = 100;
	ArmourMaxStartValue = 0;
	SpeedStartValue = 400;
	EnergyMaxStartValue = 100;
}


// Called when the game starts
void USoSASComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void USoSASComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LoopOverCurrentASEffectsArrays();
}


void USoSASComponent::LoopOverCurrentASEffectsArrays()
{
	for (FASEffectData* Effect : CurrentPositiveEffects)
	{
		CheckASEffectStatus(Effect);
	}

	for (FASEffectData* Effect : CurrentNeutralEffects)
	{
		CheckASEffectStatus(Effect);
	}

	for (FASEffectData* Effect : CurrentNegativeEffects)
	{
		CheckASEffectStatus(Effect);
	}
}


void USoSASComponent::CheckASEffectStatus(FASEffectData* Effect)
{
	// Check if effect should expire
	float EffectElapsedTime = GetWorld()->GetTimeSeconds() - Effect->EffectStartTime;
	if (EffectElapsedTime >= Effect->EffectDuration)
	{
		EndASEffect(Effect);
	}

	// Check if effect should tick
	float TimeElapsedSinceLastTick = GetWorld()->GetTimeSeconds() - Effect->TimeSinceLastTick;
	if (TimeElapsedSinceLastTick >= Effect->TickRate)
	{
		Effect->TimeSinceLastTick = GetWorld()->GetTimeSeconds();

		CheckASEffectValue(Effect);
	}
}


void USoSASComponent::CheckASEffectValue(FASEffectData* Effect)
{
	float NewValue = Effect->EffectValue * Effect->CurrentStacks;

	switch (Effect->EffectValueType)
	{
	case EASEffectValueType::Additive:
		Effect->TotalValue += NewValue;

		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeTempAdditiveValues(Effect->AttributeToEffect, NewValue);
		}
		else
		{
			
		}

		break;
	case EASEffectValueType::Multiplicative:
		Effect->TotalValue += NewValue;
		
		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeTempMultiplierValues(Effect->AttributeToEffect, NewValue);
		}
		else
		{

		}
		break;
	case EASEffectValueType::Subtractive:
		Effect->TotalValue += NewValue;

		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeTempMultiplierValues(Effect->AttributeToEffect, -NewValue);
		}
		else
		{

		}
		break;
	default:
		break;
	}
}


void USoSASComponent::AddValueToASAttributeTempAdditiveValues(EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		ASAttributeTempAdditiveValues[0] += Value;
		break;
	case EASAttributeName::HealthCurrent:
		ASAttributeTempAdditiveValues[1] += Value;
		break;
	case EASAttributeName::ArmourMax:
		ASAttributeTempAdditiveValues[2] += Value;
		break;
	case EASAttributeName::ArmourCurrent:
		ASAttributeTempAdditiveValues[3] += Value;
		break;
	case EASAttributeName::Speed:
		ASAttributeTempAdditiveValues[4] += Value;
		break;
	case EASAttributeName::EnergyMax:
		ASAttributeTempAdditiveValues[5] += Value;
		break;
	case EASAttributeName::EnergyCurrent:
		ASAttributeTempAdditiveValues[6] += Value;
		break;
	case EASAttributeName::EASATTRIBUTENAME_NR_ITEMS:
		break;
	default:
		break;
	}
}


void USoSASComponent::AddValueToASAttributeTempMultiplierValues(EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		ASAttributeTempMultiplierValues[0] += Value;
		break;
	case EASAttributeName::HealthCurrent:
		ASAttributeTempMultiplierValues[1] += Value;
		break;
	case EASAttributeName::ArmourMax:
		ASAttributeTempMultiplierValues[2] += Value;
		break;
	case EASAttributeName::ArmourCurrent:
		ASAttributeTempMultiplierValues[3] += Value;
		break;
	case EASAttributeName::Speed:
		ASAttributeTempMultiplierValues[4] += Value;
		break;
	case EASAttributeName::EnergyMax:
		ASAttributeTempMultiplierValues[5] += Value;
		break;
	case EASAttributeName::EnergyCurrent:
		ASAttributeTempMultiplierValues[6] += Value;
		break;
	case EASAttributeName::EASATTRIBUTENAME_NR_ITEMS:
		break;
	default:
		break;
	}
}


void USoSASComponent::AddValueToASAttribute(EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:		
		SetASAttribute(Attribute, CharacterData->HealthMaxValue + Value);
		break;
	case EASAttributeName::HealthCurrent:
		SetASAttribute(Attribute, CharacterData->HealthCurrentValue + Value);
		break;
	case EASAttributeName::ArmourMax:
		SetASAttribute(Attribute, CharacterData->ArmourMaxValue + Value);
		break;
	case EASAttributeName::ArmourCurrent:
		SetASAttribute(Attribute, CharacterData->ArmourCurrentValue + Value);
		break;
	case EASAttributeName::Speed:
		SetASAttribute(Attribute, CharacterData->SpeedCurrentValue + Value);
		break;
	case EASAttributeName::EnergyMax:
		SetASAttribute(Attribute, CharacterData->EnergyMaxValue + Value);
		break;
	case EASAttributeName::EnergyCurrent:
		SetASAttribute(Attribute, CharacterData->EnergyCurrentValue + Value);
		break;
	case EASAttributeName::EASATTRIBUTENAME_NR_ITEMS:
		break;
	default:
		break;
	}
}


void USoSASComponent::AddASEffectToArray(FASEffectData* EffectToAdd)
{
	switch (EffectToAdd->EffectType)
	{
	case EASEffectType::Positive:
		CurrentPositiveEffects.Add(EffectToAdd);
		break;
	case EASEffectType::Neutral:
		CurrentNeutralEffects.Add(EffectToAdd);
		break;
	case EASEffectType::Negative:
		CurrentNegativeEffects.Add(EffectToAdd);
		break;
	default:
		break;
	}
}


void USoSASComponent::RemoveASEffectFromArray(FASEffectData* EffectToRemove)
{
	switch (EffectToRemove->EffectType)
	{
	case EASEffectType::Positive:
		CurrentPositiveEffects.Remove(EffectToRemove);
		break;
	case EASEffectType::Neutral:
		CurrentNeutralEffects.Remove(EffectToRemove);
		break;
	case EASEffectType::Negative:
		CurrentNegativeEffects.Remove(EffectToRemove);
		break;
	default:
		break;
	}
}


void USoSASComponent::EndASEffect(FASEffectData* EffectToEnd)
{
	if (EffectToEnd->bTemporaryModifier)
	{
		// TODO reset attribute
	}

	RemoveASEffectFromArray(EffectToEnd);
}


////////////////////////////////////////////////
// Getters and Setters


void USoSASComponent::SetASAttribute(EASAttributeName AtrributeToSet, float Value)
{
	switch (AtrributeToSet)
	{
	case EASAttributeName::HealthMax:
		CharacterData->HealthMaxValue = FMath::Max(1.0f, Value);
		break;
	case EASAttributeName::HealthCurrent:
		CharacterData->HealthCurrentValue = FMath::Min(CharacterData->HealthMaxValue, Value);
		break;
	case EASAttributeName::ArmourMax:
		CharacterData->ArmourMaxValue = FMath::Max(0.0f, Value);
		break;
	case EASAttributeName::ArmourCurrent:
		CharacterData->ArmourCurrentValue = FMath::Min(CharacterData->ArmourMaxValue, Value);
		break;
	case EASAttributeName::Speed:
		CharacterData->SpeedCurrentValue = FMath::Max(0.0f, Value);
		break;
	case EASAttributeName::EnergyMax:
		CharacterData->EnergyMaxValue = FMath::Max(0.0f, Value);
		break;
	case EASAttributeName::EnergyCurrent:
		CharacterData->EnergyCurrentValue = FMath::Min(CharacterData->EnergyMaxValue, Value);
		break;
	default:
		break;
	}
}


float USoSASComponent::GetASAttribute(EASAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EASAttributeName::HealthMax:
		return CharacterData->HealthMaxValue;
		break;
	case EASAttributeName::HealthCurrent:
		return CharacterData->HealthCurrentValue;
		break;
	case EASAttributeName::ArmourMax:
		return CharacterData->ArmourMaxValue;
		break;
	case EASAttributeName::ArmourCurrent:
		return CharacterData->ArmourCurrentValue;
		break;
	case EASAttributeName::Speed:
		return CharacterData->SpeedCurrentValue;
		break;
	case EASAttributeName::EnergyMax:
		return CharacterData->EnergyMaxValue;
		break;
	case EASAttributeName::EnergyCurrent:
		return CharacterData->EnergyCurrentValue;
		break;
	default:
		return -1;
		break;
	}
}


TArray<FASEffectData*>& USoSASComponent::GetCurrentEffectsArray(EASEffectType EffectType)
{
	switch (EffectType)
	{
	case EASEffectType::Positive:
		return CurrentPositiveEffects;
		break;
	case EASEffectType::Neutral:
		return CurrentNeutralEffects;
		break;
	case EASEffectType::Negative:
		return CurrentNegativeEffects;
		break;
	default:
		return DefaultArray;
		break;
	}
}

