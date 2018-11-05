// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSAbilityBase.h"
#include "Engine/Classes/Animation/AnimationAsset.h"
#include "Engine/World.h"


USoSAbilityBase::USoSAbilityBase()
{
	AbilityIcon = CreateDefaultSubobject<UTexture2D>(TEXT("Icon"));
	LastTimeActivated = -99999;
	Cooldown = 0.001f;
	ChargeTime = 0.001f;
	MaxCharges = 1;
	bComboReady = false;
}


bool USoSAbilityBase::StartAbility_Implementation(AActor* Source, ASoSWeaponBase* Weapon, float ClassSpecificFloatValue)
{
	return true;
}


bool USoSAbilityBase::ReleaseAbility_Implementation(AActor* Source, ASoSWeaponBase* Weapon, float ClassSpecificFloatValue)
{
	return true;
}


void USoSAbilityBase::ReadyCombo_Implementation()
{
	bComboReady = true;
}


void USoSAbilityBase::EndCombo_Implementation()
{
	bComboReady = false;
}


EAbilityCastType USoSAbilityBase::GetCastType()
{
	return CastType;
}


float USoSAbilityBase::GetLastTimeActivated() const
{
	return LastTimeActivated;
}


float USoSAbilityBase::GetLastChargeRemainder() const
{
	return LastChargeRemainder;
}

float USoSAbilityBase::GetCooldown() const
{
	return Cooldown;
}


float USoSAbilityBase::GetCost() const
{
	return Cost;
}


bool USoSAbilityBase::GetHasCharges() const
{
	return bHasCharges;
}


float USoSAbilityBase::GetChargeTime() const
{
	return ChargeTime;
}


bool USoSAbilityBase::GetComboReady() const
{
	return bComboReady;
}

int32 USoSAbilityBase::GetMaxCharges() const
{
	return MaxCharges;
}


int32 USoSAbilityBase::GetCurrentCharges() const
{
	return CurrentCharges;
}


UTexture2D* USoSAbilityBase::GetAbilityIcon() const
{
	return AbilityIcon;
}


EAbilityResourceType USoSAbilityBase::GetResourceType() const
{
	return ResourceType;
}


USoSCombatComponent* USoSAbilityBase::GetOwningCombatComp() const
{
	return OwningCombatComp;
}


void USoSAbilityBase::SetLastTimeActivated(float NewTime)
{
	LastTimeActivated = NewTime;
}


void USoSAbilityBase::SetLastChargeRemainder(float NewRemainder)
{
	LastChargeRemainder = NewRemainder;
}


void USoSAbilityBase::SetCurrentCharges(int32 Charges)
{
	CurrentCharges = FMath::Clamp(Charges, 0, MaxCharges);
}


void USoSAbilityBase::SetComboReady(bool bNewComboReady)
{
	bComboReady = bNewComboReady;
}


void USoSAbilityBase::SetOwningCombatComp(USoSCombatComponent* CombatComp)
{
	OwningCombatComp = CombatComp;
}
