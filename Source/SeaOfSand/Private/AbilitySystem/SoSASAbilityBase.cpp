// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASAbilityBase.h"
#include "Engine/Classes/Animation/AnimationAsset.h"
#include "Engine/World.h"


USoSASAbilityBase::USoSASAbilityBase()
{
	AbilityIcon = CreateDefaultSubobject<UTexture2D>(TEXT("Icon"));
	LastTimeActivated = -99999;
	Cooldown = 0.001f;
	ChargeTime = 0.001f;
	MaxCharges = 1;
}

float USoSASAbilityBase::GetLastTimeActivated() const
{
	return LastTimeActivated;
}


float USoSASAbilityBase::GetLastChargeRemainder() const
{
	return LastChargeRemainder;
}

float USoSASAbilityBase::GetCooldown() const
{
	return Cooldown;
}


float USoSASAbilityBase::GetCost() const
{
	return Cost;
}


bool USoSASAbilityBase::GetbHasCharges() const
{
	return bHasCharges;
}


float USoSASAbilityBase::GetChargeTime() const
{
	return ChargeTime;
}


int32 USoSASAbilityBase::GetMaxCharges() const
{
	return MaxCharges;
}


int32 USoSASAbilityBase::GetCurrentCharges() const
{
	return CurrentCharges;
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


void USoSASAbilityBase::SetLastChargeRemainder(float NewRemainder)
{
	LastChargeRemainder = NewRemainder;
}


void USoSASAbilityBase::SetCurrentCharges(int32 Charges)
{
	CurrentCharges = FMath::Clamp(Charges, 0, MaxCharges);
}
