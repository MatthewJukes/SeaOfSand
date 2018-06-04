// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerInventory.h"
#include "SoSPlayerCharacter.h"
#include "BaseWeapon.h"
#include "SoSRifle.h"
#include "SoSPistol.h"
#include "SoSShotgun.h"

// Sets default values for this component's properties
UPlayerInventory::UPlayerInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Socket names
	RightHandAttachPoint = TEXT("RightHandSocket");
	RifleHolsterAttachPoint = TEXT("RifleHolsterSocket");
	PistolHolsterAttachPoint = TEXT("PistolHolsterSocket");
	ShotgunHolsterAttachPoint = TEXT("ShotgunHolsterSocket");
}


// Called when the game starts
void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();

	// Get the player character
	PlayerCharacter = Cast<ASoSPlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got Player"));
		SpawnWeapon(PistolBlueprint);
		SpawnWeapon(RifleBlueprint);
		SpawnWeapon(ShotgunBlueprint);

		CurrentWeapon = EquippedWeapons[0];
		CurrentWeaponArrayID = 0;
	}
}

void UPlayerInventory::SpawnWeapon(TSubclassOf<ABaseWeapon> WeaponToSpawn)
{
	// Spawn new weapon
	FActorSpawnParameters SpawnParams;
	ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponToSpawn, SpawnParams);

	// Add new weapon to array of equipped weapons
	EquippedWeapons.Add(NewWeapon);

	// Attach new weapon to appropriate holster
	AttachWeaponToSocket(NewWeapon);
}


void UPlayerInventory::AttachWeaponToSocket(ABaseWeapon* Weapon, bool bDrawWeapon)
{
	if (bDrawWeapon)
	{
		Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandAttachPoint);
	}
	else
	{
		switch (Weapon->WeaponType)
		{
		case EWeaponType::Pistol:
			Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, PistolHolsterAttachPoint);
			break;
		case EWeaponType::Rifle:
			Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleHolsterAttachPoint);
			break;
		case EWeaponType::Shotgun:
			Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShotgunHolsterAttachPoint);
			break;
		default:
			break;
		}
	}
}

void UPlayerInventory::HolsterUnholster()
{ 
	if (CurrentWeapon) // TODO refine and refactor this
	{
		if (bWeaponIsDrawn) // Holster weapon
		{
			if (PlayerCharacter->bIsAiming) { PlayerCharacter->AimEnd(); }
			AttachWeaponToSocket(CurrentWeapon);
			bWeaponIsDrawn = false;
			PlayerCharacter->OffsetCamera(false);

			CurrentWeapon->InterruptReload();

			// Update character movement settings
			PlayerCharacter->SetPlayerSpeed(1.f);
			PlayerCharacter->SetPlayerMovementType(true, false);
		}
		else // Unholster weapon
		{
			AttachWeaponToSocket(CurrentWeapon, true);
			bWeaponIsDrawn = true;
			PlayerCharacter->OffsetCamera(true);

			// Update character movement settings
			PlayerCharacter->SetPlayerSpeed(CurrentWeapon->WeaponDrawnSpeedMultiplier);
			PlayerCharacter->SetPlayerMovementType(false, true);
		}
	} 
}

void UPlayerInventory::CycleWeapons(bool bNextWeapon)
{
	bool bWeaponWasDrawn = bWeaponIsDrawn;
	if (bWeaponIsDrawn) { HolsterUnholster(); }
	
	int32 ArrayLenth = EquippedWeapons.Num();

	if (bNextWeapon) // Get next weapon
	{
		CurrentWeaponArrayID = (CurrentWeaponArrayID + 1) % ArrayLenth;
		CurrentWeapon = EquippedWeapons[CurrentWeaponArrayID];
	}
	else // Get prev weapon
	{
		CurrentWeaponArrayID = (CurrentWeaponArrayID + (ArrayLenth - 1)) % ArrayLenth;
		CurrentWeapon = EquippedWeapons[CurrentWeaponArrayID];
	}

	if (bWeaponWasDrawn) { HolsterUnholster(); }
}
