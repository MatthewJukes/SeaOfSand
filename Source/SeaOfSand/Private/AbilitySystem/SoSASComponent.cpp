// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "SoSASAbilityBase.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	HealthMaxStartValue = 100;
	ArmourMaxStartValue = 0;
	EnergyMaxStartValue = 100;
	SpeedStartValue = 400;	
	
	ASAttributeBaseValues.HealthMaxValue = HealthMaxStartValue;
	ASAttributeBaseValues.HealthCurrentValue = HealthMaxStartValue;
	ASAttributeBaseValues.ArmourMaxValue = ArmourMaxStartValue;
	ASAttributeBaseValues.ArmourCurrentValue = ArmourMaxStartValue;
	ASAttributeBaseValues.EnergyMaxValue = EnergyMaxStartValue;
	ASAttributeBaseValues.HealthCurrentValue = EnergyMaxStartValue;
	ASAttributeBaseValues.SpeedValue = SpeedStartValue;

	ASAttributeTempMultiplierValues.HealthMaxValue = 1;
	ASAttributeTempMultiplierValues.HealthCurrentValue = 1;
	ASAttributeTempMultiplierValues.ArmourMaxValue = 1;
	ASAttributeTempMultiplierValues.ArmourCurrentValue = 1;
	ASAttributeTempMultiplierValues.EnergyMaxValue = 1;
	ASAttributeTempMultiplierValues.EnergyCurrentValue = 1;
	ASAttributeTempMultiplierValues.SpeedValue = 1;
}


// Called when the game starts
void USoSASComponent::BeginPlay()
{
	Super::BeginPlay();

}


void USoSASComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LoopOverCurrentASEffectsArray();
}


void USoSASComponent::LoopOverCurrentASEffectsArray()
{
	int32 Index = 0;
	TArray<int32> EffectIndexToRemove;
	for (FASEffectData& Effect : CurrentEffects)
	{
		CheckASEffectStatus(Effect);
		if (Effect.bExpired)
		{
			EffectIndexToRemove.Add(Index);
		}
		Index++;
	}
	RemoveASEffectFromArrayByIndexArray(EffectIndexToRemove);

	CalculateASAttributeTotalValues();
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), ASAttributeTotalValues.HealthCurrentValue);
	UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %f"), ASAttributeTotalValues.HealthMaxValue);
}


void USoSASComponent::CheckASEffectStatus(FASEffectData& Effect)
{
	// Check if effect should expire
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float EffectElapsedTime = GetWorld()->GetTimeSeconds() - Effect.EffectStartTime;
	if (EffectElapsedTime >= Effect.EffectDuration)
	{
		EndASEffect(Effect);
		Effect.bExpired = true;
		return;
	}

	// Check if effect should tick
	float TimeElapsedSinceLastTick = GetWorld()->GetTimeSeconds() - Effect.LastTickTime;
	if (TimeElapsedSinceLastTick >= Effect.TickRate)
	{
		Effect.LastTickTime = GetWorld()->GetTimeSeconds();
		
		HandleASEffectValue(Effect, false);
	}
}


void USoSASComponent::HandleASEffectValue(FASEffectData& Effect, bool bUseTotalValue)
{
	float NewValue = bUseTotalValue ? Effect.TotalValue : Effect.EffectValue * Effect.CurrentStacks;

	switch (Effect.EffectValueType)
	{
	case EASEffectValueType::Additive:
		Effect.TotalValue += NewValue;

		if (Effect.bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempAdditiveValues, Effect.AttributeToEffect, NewValue);
		}
		else
		{
			AddValueToASAttributeData(&ASAttributeBaseValues, Effect.AttributeToEffect, NewValue);
		}

		break;
	case EASEffectValueType::Multiplicative:
		Effect.TotalValue += NewValue;
		NewValue *= 0.1f;
		
		if (Effect.bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempMultiplierValues, Effect.AttributeToEffect, NewValue);
		}
		else
		{
			MultiplyASAttributeDataByValue(&ASAttributeBaseValues, Effect.AttributeToEffect, 1 + NewValue);
		}
		break;
	case EASEffectValueType::Subtractive:
		Effect.TotalValue += NewValue;
		NewValue *= 0.1f;

		if (Effect.bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempMultiplierValues, Effect.AttributeToEffect, -NewValue);
		}
		else
		{
			MultiplyASAttributeDataByValue(&ASAttributeBaseValues, Effect.AttributeToEffect, 1 - NewValue);
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


void USoSASComponent::MultiplyASAttributeDataByValue(FASAttributeData* AttributeData, EASAttributeName Attribute, float Value)
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


void USoSASComponent::CalculateASAttributeTotalValues()
{
	ASAttributeTotalValues.HealthMaxValue = FMath::Max(1.0f, ASAttributeBaseValues.HealthMaxValue * ASAttributeTempMultiplierValues.HealthMaxValue + ASAttributeTempAdditiveValues.HealthMaxValue);

	ASAttributeTotalValues.HealthCurrentValue = FMath::Clamp(ASAttributeBaseValues.HealthCurrentValue * ASAttributeTempMultiplierValues.HealthCurrentValue + ASAttributeTempAdditiveValues.HealthCurrentValue, 0.0f, ASAttributeTotalValues.HealthMaxValue);

	ASAttributeTotalValues.ArmourMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.ArmourMaxValue * ASAttributeTempMultiplierValues.ArmourMaxValue + ASAttributeTempAdditiveValues.ArmourMaxValue);

	ASAttributeTotalValues.ArmourCurrentValue = FMath::Clamp(ASAttributeBaseValues.ArmourCurrentValue * ASAttributeTempMultiplierValues.ArmourCurrentValue + ASAttributeTempAdditiveValues.ArmourCurrentValue, 0.0f, ASAttributeTotalValues.ArmourMaxValue);

	ASAttributeTotalValues.EnergyMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.EnergyMaxValue * ASAttributeTempMultiplierValues.EnergyMaxValue + ASAttributeTempAdditiveValues.EnergyMaxValue);

	ASAttributeTotalValues.EnergyCurrentValue = FMath::Clamp(ASAttributeBaseValues.EnergyCurrentValue * ASAttributeTempMultiplierValues.EnergyCurrentValue + ASAttributeTempAdditiveValues.EnergyCurrentValue, 0.0f, ASAttributeTotalValues.EnergyMaxValue);

	ASAttributeTotalValues.SpeedValue = FMath::Max(0.0f, ASAttributeBaseValues.SpeedValue * ASAttributeTempMultiplierValues.SpeedValue + ASAttributeTempAdditiveValues.SpeedValue);
}


void USoSASComponent::AddASEffectToArray(FASEffectData& EffectToAdd)
{
	CurrentEffects.Add(EffectToAdd);
	for (EASTag Tag : EffectToAdd.EffectAppliesTags)
	{
		CurrentTags.Add(Tag);
	}
}


void USoSASComponent::RemoveASEffectFromArrayByIndex(int32 Index)
{
	CurrentEffects.RemoveAt(Index);
}


void USoSASComponent::RemoveASEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove)
{
	for (int32 Index : EffectIndexesToRemove)
	{
		RemoveASEffectFromArrayByIndex(Index);
	}
}


void USoSASComponent::EndASEffect(FASEffectData& EffectToEnd)
{
	if (EffectToEnd.bTemporaryModifier)
	{
		EffectToEnd.TotalValue = -EffectToEnd.TotalValue;
		HandleASEffectValue(EffectToEnd, true);
	}

	for (EASTag Tag : EffectToEnd.EffectAppliesTags)
	{
		CurrentTags.Remove(Tag);
	}
}


void USoSASComponent::UseAbility(USoSASAbilityBase* Ability)
{
	if (Ability == nullptr)
	{
		return;
	}

	Ability->StartAbility(GetOwner(), GetOwner(), GetWorld()->GetTimeSeconds());
}

////////////////////////////////////////////////
// Getters and Setters


float USoSASComponent::GetASAttributeDataValue(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const
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


float USoSASComponent::GetASAttributeTotalValue(EASAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EASAttributeName::HealthMax:
		return ASAttributeTotalValues.HealthMaxValue;
		break;
	case EASAttributeName::HealthCurrent:
		return ASAttributeTotalValues.HealthCurrentValue;
		break;
	case EASAttributeName::ArmourMax:
		return ASAttributeTotalValues.ArmourMaxValue;
		break;
	case EASAttributeName::ArmourCurrent:
		return ASAttributeTotalValues.ArmourCurrentValue;
		break;
	case EASAttributeName::EnergyMax:
		return ASAttributeTotalValues.EnergyMaxValue;
		break;
	case EASAttributeName::EnergyCurrent:
		return ASAttributeTotalValues.EnergyCurrentValue;
		break;
	case EASAttributeName::Speed:
		return ASAttributeTotalValues.SpeedValue;
		break;
	default:
		return -1;
		break;
	}
}

TArray<FASEffectData>& USoSASComponent::GetCurrentEffectsArray()
{
	return CurrentEffects;
}

TArray<EASTag>& USoSASComponent::GetCurrentTags()
{
	return CurrentTags;
}

