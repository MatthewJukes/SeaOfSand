// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASAbilityBase.h"
#include "Engine/World.h"


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


EASResourceType USoSASAbilityBase::GetResourceType() const
{
	return ResourceType;
}


void USoSASAbilityBase::SetLastTimeActivated(float NewTime)
{
	LastTimeActivated = NewTime;
}
