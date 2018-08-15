// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{
	ASAttributeTempMultiplierValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));
	ASAttributeTempAdditiveValues.Init(0.0f, int32(EASAttributeName::EASATTRIBUTENAME_NR_ITEMS));
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

		}
		else
		{

		}

		break;
	case EASEffectValueType::Multiplicative:
		Effect->TotalValue += NewValue;

		break;
	case EASEffectValueType::Subtractive:
		Effect->TotalValue += NewValue;

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
	case EASAttributeName::SpeedBase:
		ASAttributeTempAdditiveValues[4] += Value;
		break;
	case EASAttributeName::SpeedCurrent:
		ASAttributeTempAdditiveValues[5] += Value;
		break;
	case EASAttributeName::EnergyMax:
		ASAttributeTempAdditiveValues[6] += Value;
		break;
	case EASAttributeName::EnergyCurrent:
		ASAttributeTempAdditiveValues[7] += Value;
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
		
		break;
	case EASAttributeName::HealthCurrent:
		break;
	case EASAttributeName::ArmourMax:
		break;
	case EASAttributeName::ArmourCurrent:
		break;
	case EASAttributeName::SpeedBase:
		break;
	case EASAttributeName::SpeedCurrent:
		break;
	case EASAttributeName::EnergyMax:
		break;
	case EASAttributeName::EnergyCurrent:
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

}


////////////////////////////////////////////////
// Getters and Setters


void USoSASComponent::SetASAttribute(EASAttributeName AtrributeToSet, float Value)
{
	switch (AtrributeToSet)
	{
	case EASAttributeName::HealthMax:
		//HealthBaseValue = Value;
		break;
	case EASAttributeName::HealthCurrent:
		//HealthCurrentValue = Value;
		break;
	case EASAttributeName::ArmourMax:
		break;
	case EASAttributeName::ArmourCurrent:
		break;
	case EASAttributeName::SpeedBase:
		//SpeedBaseValue = Value;
		break;
	case EASAttributeName::SpeedCurrent:
		//SpeedCurrentValue = Value;
		break;
	case EASAttributeName::EnergyMax:
		break;
	case EASAttributeName::EnergyCurrent:
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
		return 1;
		break;
	case EASAttributeName::HealthCurrent:
		return 1;
		break;
	case EASAttributeName::ArmourMax:
		return 1;
		break;
	case EASAttributeName::ArmourCurrent:
		return 1;
		break;
	case EASAttributeName::SpeedBase:
		return 1;
		break;
	case EASAttributeName::SpeedCurrent:
		return 1;
		break;
	case EASAttributeName::EnergyMax:
		return 1;
		break;
	case EASAttributeName::EnergyCurrent:
		return 1;
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

