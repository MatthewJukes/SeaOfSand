// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASAbilityBase.h"
#include "Engine/World.h"


USoSASAbilityBase::USoSASAbilityBase()
{
	AbilityIcon = CreateDefaultSubobject<UTexture2D>(TEXT("Icon"));
	LastTimeActivated = -99999;
}

float USoSASAbilityBase::GetLastTimeActivated() const
{
	return LastTimeActivated;
}


float USoSASAbilityBase::GetCooldown() const
{
	return Cooldown;
}


float USoSASAbilityBase::GetCost() const
{
	return Cost;
}


UTexture2D* USoSASAbilityBase::GetAbilityIcon() const
{
	return AbilityIcon;
}

EASResourceType USoSASAbilityBase::GetResourceType() const
{
	return ResourceType;
}


void USoSASAbilityBase::SetLastTimeActivated(float NewTime)
{
	LastTimeActivated = NewTime;
}
