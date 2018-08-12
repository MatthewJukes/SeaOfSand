// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "SoSASEffect.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{

}


// Called when the game starts
void USoSASComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USoSASComponent::AddASEffectToArray(FASEffectData* EffectToAdd)
{
	switch (EffectToAdd->EffectType)
	{
	case EASEffectType::Positive:
		break;
	case EASEffectType::Neutral:
		break;
	case EASEffectType::Negative:
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////
// Getters and Setters

void USoSASComponent::SetASAttribute(EASAttributeName AtrributeToSet, float Value)
{
	switch (AtrributeToSet)
	{
	case EASAttributeName::HealthBase:
		HealthBaseValue = Value;
		break;
	case EASAttributeName::HealthCurrent:
		HealthCurrentValue = Value;
		break;
	case EASAttributeName::ArmourBase:
		break;
	case EASAttributeName::ArmourCurrent:
		break;
	case EASAttributeName::SpeedBase:
		SpeedBaseValue = Value;
		break;
	case EASAttributeName::SpeedCurrent:
		SpeedCurrentValue = Value;
		break;
	case EASAttributeName::EnergyBase:
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
	case EASAttributeName::HealthBase:
		return HealthBaseValue;
		break;
	case EASAttributeName::HealthCurrent:
		return HealthCurrentValue;
		break;
	case EASAttributeName::ArmourBase:
		break;
	case EASAttributeName::ArmourCurrent:
		break;
	case EASAttributeName::SpeedBase:
		return SpeedBaseValue;
		break;
	case EASAttributeName::SpeedCurrent:
		return SpeedCurrentValue;
		break;
	case EASAttributeName::EnergyBase:
		break;
	case EASAttributeName::EnergyCurrent:
		break;
	default:
		return -1;
		break;
	}
}

