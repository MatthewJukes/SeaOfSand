// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSWeaponBase.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
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
}


void ASoSWeaponBase::StartAttack()
{

}

void ASoSWeaponBase::EndAttack()
{

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


USkeletalMeshComponent* ASoSWeaponBase::GetWeaponMesh() const
{
	return WeaponMesh;
}


FName ASoSWeaponBase::GetProjectileOriginSocketName()
{
	return ProjectileOriginSocketName;
}


void ASoSWeaponBase::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
}

