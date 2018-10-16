// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSMeleeWeapon.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "Components/CapsuleComponent.h"


ASoSMeleeWeapon::ASoSMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	DamageCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DamageCapsule"));
	DamageCapsule->AttachToComponent(RootComponent);

	WeaponType = EWeaponType::Melee;
}


void ASoSMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();	
}


void ASoSMeleeWeapon::StartAttack()
{
	if (PlayerCharacter->UseAbility(WeaponAbilities.AbilityWeaponPrimary))
	{
		SetWeaponState(EWeaponState::Attacking);
	}
}


void ASoSMeleeWeapon::EndAttack()
{
	Super::EndAttack();
}


UCapsuleComponent* ASoSMeleeWeapon::GetDamageCapsule() const
{
	return DamageCapsule;
}

