// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSMeleeWeapon.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"


ASoSMeleeWeapon::ASoSMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

}


void ASoSMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASoSMeleeWeapon::StartAttack()
{
	if (PlayerCharacter->UseAbility(WeaponAbilities.AbilityWeaponPrimaryInstance))
	{
		SetWeaponState(EWeaponState::Attacking);
	}
}


void ASoSMeleeWeapon::EndAttack()
{
	Super::EndAttack();
}

