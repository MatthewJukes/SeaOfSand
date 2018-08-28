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

	LoopOverCurrentASEffectsArrays();
}


void USoSASComponent::LoopOverCurrentASEffectsArrays()
{
	for (FASEffectData Effect : CurrentPositiveEffects)
	{
		//CheckASEffectStatus(Effect);
	}

	for (FASEffectData Effect : CurrentNeutralEffects)
	{
		//CheckASEffectStatus(Effect);
	}

	for (FASEffectData Effect : CurrentNegativeEffects)
	{
		CheckASEffectStatus(&Effect);
	}

	CalculateASAttributeTotalValues();
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), ASAttributeTotalValues.HealthCurrentValue);
}


void USoSASComponent::CheckASEffectStatus(FASEffectData* Effect)
{
	// Check if effect should expire
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float EffectElapsedTime = GetWorld()->GetTimeSeconds() - Effect->EffectStartTime;
	//UE_LOG(LogTemp, Warning, TEXT("Effect Name: %s Effect Duration: %f"), *Effect->EffectName.ToString(), Effect->EffectDuration);
	if (EffectElapsedTime >= Effect->EffectDuration)
	{
		//EndASEffect(Effect);
		//return;
	}

	// Check if effect should tick
	float TimeElapsedSinceLastTick = GetWorld()->GetTimeSeconds() - Effect->TimeSinceLastTick;
	if (TimeElapsedSinceLastTick >= Effect->TickRate)
	{
		Effect->TimeSinceLastTick = GetWorld()->GetTimeSeconds();
		
		HandleASEffectValue(Effect, false);
	}
}


void USoSASComponent::HandleASEffectValue(FASEffectData* Effect, bool bUseTotalValue)
{
	float NewValue = bUseTotalValue ? Effect->TotalValue : Effect->EffectValue * Effect->CurrentStacks;

	switch (Effect->EffectValueType)
	{
	case EASEffectValueType::Additive:
		Effect->TotalValue += NewValue;

		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempAdditiveValues, Effect->AttributeToEffect, NewValue);
		}
		else
		{
			AddValueToASAttributeData(&ASAttributeBaseValues, Effect->AttributeToEffect, NewValue);
		}

		break;
	case EASEffectValueType::Multiplicative:
		Effect->TotalValue += NewValue;
		NewValue *= 0.1f;
		
		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempMultiplierValues, Effect->AttributeToEffect, NewValue);
		}
		else
		{
			MultiplyASAttributeDataByValue(&ASAttributeBaseValues, Effect->AttributeToEffect, 1 + NewValue);
		}
		break;
	case EASEffectValueType::Subtractive:
		Effect->TotalValue += NewValue;
		NewValue *= 0.1f;

		if (Effect->bTemporaryModifier)
		{
			AddValueToASAttributeData(&ASAttributeTempMultiplierValues, Effect->AttributeToEffect, -NewValue);
		}
		else
		{
			MultiplyASAttributeDataByValue(&ASAttributeBaseValues, Effect->AttributeToEffect, 1 - NewValue);
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

	ASAttributeTotalValues.HealthCurrentValue = FMath::Min(ASAttributeTotalValues.HealthMaxValue, ASAttributeBaseValues.HealthCurrentValue * ASAttributeTempMultiplierValues.HealthCurrentValue + ASAttributeTempAdditiveValues.HealthCurrentValue);

	ASAttributeTotalValues.ArmourMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.ArmourMaxValue * ASAttributeTempMultiplierValues.ArmourMaxValue + ASAttributeTempAdditiveValues.ArmourMaxValue);

	ASAttributeTotalValues.ArmourCurrentValue = FMath::Min(ASAttributeTotalValues.ArmourMaxValue, ASAttributeBaseValues.ArmourCurrentValue * ASAttributeTempMultiplierValues.ArmourCurrentValue + ASAttributeTempAdditiveValues.ArmourCurrentValue);

	ASAttributeTotalValues.EnergyMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.EnergyMaxValue * ASAttributeTempMultiplierValues.EnergyMaxValue + ASAttributeTempAdditiveValues.EnergyMaxValue);

	ASAttributeTotalValues.EnergyCurrentValue = FMath::Min(ASAttributeTotalValues.EnergyMaxValue, ASAttributeBaseValues.EnergyCurrentValue * ASAttributeTempMultiplierValues.EnergyCurrentValue + ASAttributeTempAdditiveValues.EnergyCurrentValue);

	ASAttributeTotalValues.SpeedValue = FMath::Max(0.0f, ASAttributeBaseValues.SpeedValue * ASAttributeTempMultiplierValues.SpeedValue + ASAttributeTempAdditiveValues.SpeedValue);
}


void USoSASComponent::AddASEffectToArray(FASEffectData* EffectToAdd)
{
	switch (EffectToAdd->EffectType)
	{
	case EASEffectType::Positive:
		CurrentPositiveEffects.Add(*EffectToAdd);
		break;
	case EASEffectType::Neutral:
		CurrentNeutralEffects.Add(*EffectToAdd);
		break;
	case EASEffectType::Negative:
		CurrentNegativeEffects.Add(*EffectToAdd);
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
		//CurrentPositiveEffects.Remove(*EffectToRemove);
		break;
	case EASEffectType::Neutral:
		//CurrentNeutralEffects.Remove(*EffectToRemove);
		break;
	case EASEffectType::Negative:
		//CurrentNegativeEffects.Remove(*EffectToRemove);
		break;
	default:
		break;
	}
}


void USoSASComponent::EndASEffect(FASEffectData* EffectToEnd)
{
	if (EffectToEnd->bTemporaryModifier)
	{
		EffectToEnd->TotalValue = -EffectToEnd->TotalValue;
		HandleASEffectValue(EffectToEnd, true);
	}

	RemoveASEffectFromArray(EffectToEnd);
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

TArray<FASEffectData>& USoSASComponent::GetCurrentEffectsArray(EASEffectType EffectType)
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

