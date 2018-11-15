// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBasicAttackAbilityMelee.h"
#include "SoSCombatComponent.h"
#include "SoSCharacterBase.h"
#include "SoSInventoryComponent.h"
#include "SoSMeleeWeapon.h"
#include "Components/CapsuleComponent.h"


void USoSBasicAttackAbilityMelee::InitializeAbility_Implementation()
{
	MeleeWeapon = GetOwningCombatComp()->GetOwningCharacter()->GetCharacterInventory()->GetMeleeWeapon();
	if (MeleeWeapon != nullptr)
	{
		MeleeWeapon->GetDamageCapsule()->OnComponentBeginOverlap.AddDynamic(this, &USoSBasicAttackAbilityMelee::MeleeHit);
	}

	IgnoreTargets.Reserve(6);
}

void USoSBasicAttackAbilityMelee::HitboxActivate_Implementation()
{
	if (MeleeWeapon == nullptr)
	{
		return;
	}

	MeleeWeapon->GetDamageCapsule()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void USoSBasicAttackAbilityMelee::HitboxDeactivate_Implementation()
{
	if (MeleeWeapon == nullptr)
	{
		return;
	}

	MeleeWeapon->GetDamageCapsule()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreTargets.Empty();
}

