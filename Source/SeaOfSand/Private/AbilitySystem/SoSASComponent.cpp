// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"


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

////////////////////////////////////////////////
// Getters and Setters

void USoSASComponent::SetASAttribute(EASAttributeName AtrributeToSet, float Value, float Duration)
{
	switch (AtrributeToSet)
	{
	case EASAttributeName::HealthBase:
		break;
	case EASAttributeName::HealthCurrent:
		break;
	case EASAttributeName::ArmourBase:
		break;
	case EASAttributeName::ArmourCurrent:
		break;
	case EASAttributeName::SpeedBase:
		break;
	case EASAttributeName::SpeedCurrent:
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
		break;
	}
}

