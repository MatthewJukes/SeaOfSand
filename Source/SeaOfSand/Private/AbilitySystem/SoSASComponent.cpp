// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{
	HealthMaxStartValue = 100;
	ArmourMaxStartValue = 0;
	EnergyMaxStartValue = 100;
	SpeedStartValue = 400;

	ASAttributeBaseValues->HealthMaxValue = HealthMaxStartValue;
	ASAttributeBaseValues->HealthCurrentValue = HealthMaxStartValue;
	ASAttributeBaseValues->ArmourMaxValue = ArmourMaxStartValue;
	ASAttributeBaseValues->ArmourCurrentValue = ArmourMaxStartValue;
	ASAttributeBaseValues->EnergyMaxValue = EnergyMaxStartValue;
	ASAttributeBaseValues->HealthCurrentValue = EnergyMaxStartValue;
	ASAttributeBaseValues->SpeedValue = SpeedStartValue;
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
			AddValueToASAttributeData(ASAttributeTempAdditiveValues, Effect->AttributeToEffect, NewValue);
		}
		else
		{
			AddValueToASAttributeData(ASAttributeBaseValues, Effect->AttributeToEffect, NewValue);
		}

		break;
	case EASEffectValueType::Multiplicative:
		Effect->TotalValue += NewValue;
		
		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeData(ASAttributeTempMultiplierValues, Effect->AttributeToEffect, NewValue);
		}
		else
		{
			AddMultiplierToASAttributeData(ASAttributeBaseValues, Effect->AttributeToEffect, 1 + NewValue);
		}
		break;
	case EASEffectValueType::Subtractive:
		Effect->TotalValue += NewValue;

		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeData(ASAttributeTempMultiplierValues, Effect->AttributeToEffect, -NewValue);
		}
		else
		{
			AddMultiplierToASAttributeData(ASAttributeBaseValues, Effect->AttributeToEffect, 1 - NewValue);
		}
		break;
	default:
		break;
	}
}


void USoSASComponent::AddValueToASAttributeData(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		AttributeData->HealthMaxValue += Value;
		break;
	case EASAttributeName::HealthCurrent:
		AttributeData->HealthCurrentValue += Value;
		break;
	case EASAttributeName::ArmourMax:
		AttributeData->ArmourMaxValue += Value;
		break;
	case EASAttributeName::ArmourCurrent:
		AttributeData->ArmourCurrentValue += Value;
		break;
	case EASAttributeName::EnergyMax:
		AttributeData->EnergyMaxValue += Value;
		break;
	case EASAttributeName::EnergyCurrent:
		AttributeData->EnergyCurrentValue += Value;
		break;
	case EASAttributeName::Speed:
		AttributeData->SpeedValue += Value;
		break;
	default:
		break;
	}
}


void USoSASComponent::AddMultiplierToASAttributeData(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		AttributeData->HealthMaxValue *= Value;
		break;
	case EASAttributeName::HealthCurrent:
		AttributeData->HealthCurrentValue *= Value;
		break;
	case EASAttributeName::ArmourMax:
		AttributeData->ArmourMaxValue *= Value;
		break;
	case EASAttributeName::ArmourCurrent:
		AttributeData->ArmourCurrentValue *= Value;
		break;
	case EASAttributeName::EnergyMax:
		AttributeData->EnergyMaxValue *= Value;
		break;
	case EASAttributeName::EnergyCurrent:
		AttributeData->EnergyCurrentValue *= Value;
		break;
	case EASAttributeName::Speed:
		AttributeData->SpeedValue *= Value;
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


float USoSASComponent::GetASAttribute(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EASAttributeName::HealthMax:
		return AttributeData->HealthMaxValue;
		break;
	case EASAttributeName::HealthCurrent:
		return AttributeData->HealthCurrentValue;
		break;
	case EASAttributeName::ArmourMax:
		return AttributeData->ArmourMaxValue;
		break;
	case EASAttributeName::ArmourCurrent:
		return AttributeData->ArmourCurrentValue;
		break;
	case EASAttributeName::EnergyMax:
		return AttributeData->EnergyMaxValue;
		break;
	case EASAttributeName::EnergyCurrent:
		return AttributeData->EnergyCurrentValue;
		break;
	case EASAttributeName::Speed:
		return AttributeData->SpeedValue;
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

