// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSWeaponBase.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSASTasks.h"
#include "SoSASAbilityBase.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ASoSWeaponBase::ASoSWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	ProjectileOriginSocketName = "ProjectileOriginSocket";

	WeaponState = EWeaponState::Holstered;
}


// Called when the game starts or when spawned
void ASoSWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<ASoSPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerCharacter = Cast<ASoSPlayerCharacter>(GetOwner());

	WeaponAbilities.AbilityWeaponDraw = USoSASTasks::CreateAbilityInstance(WeaponAbilities.AbilityWeaponDrawClass, PlayerCharacter->GetPlayerCombatComponent());
	WeaponAbilities.AbilityWeaponHolster = USoSASTasks::CreateAbilityInstance(WeaponAbilities.AbilityWeaponHolsterClass, PlayerCharacter->GetPlayerCombatComponent());
	WeaponAbilities.AbilityWeaponPrimary = USoSASTasks::CreateAbilityInstance(WeaponAbilities.AbilityWeaponPrimaryClass, PlayerCharacter->GetPlayerCombatComponent());
	WeaponAbilities.AbilityWeaponAlt = USoSASTasks::CreateAbilityInstance(WeaponAbilities.AbilityWeaponAltClass, PlayerCharacter->GetPlayerCombatComponent());
}


void ASoSWeaponBase::StartAttack()
{

}


void ASoSWeaponBase::EndAttack()
{
	if (GetWeaponState() == EWeaponState::Attacking)
	{
		SetWeaponState(EWeaponState::Idle);
	}
}


///////////////////////////////////////////////////
// Getters and Setters


EWeaponType ASoSWeaponBase::GetWeaponType() const
{
	return WeaponType;
}


EWeaponState ASoSWeaponBase::GetWeaponState() const
{
	return WeaponState;
}


FWeaponAbilitiesData& ASoSWeaponBase::GetWeaponAbilities()
{
	return WeaponAbilities;
}


FVector ASoSWeaponBase::GetProjectileOriginLocation() const
{
	return WeaponMesh->GetSocketLocation(ProjectileOriginSocketName);
}


USkeletalMeshComponent* ASoSWeaponBase::GetWeaponMesh() const
{
	return WeaponMesh;
}


void ASoSWeaponBase::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
}

