// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerInventory.h"
#include "SoSPlayerCharacter.h"
#include "SoSBaseWeapon.h"
#include "SoSRifle.h"
#include "SoSPistol.h"
#include "SoSShotgun.h"

// Sets default values for this component's properties
USoSPlayerInventory::USoSPlayerInventory()
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
void USoSPlayerInventory::BeginPlay()
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

void USoSPlayerInventory::SpawnWeapon(TSubclassOf<ASoSBaseWeapon> WeaponToSpawn)
{
	// Spawn new weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASoSBaseWeapon* NewWeapon = GetWorld()->SpawnActor<ASoSBaseWeapon>(WeaponToSpawn, SpawnParams);
	
	if (NewWeapon)
	{
		// Add new weapon to array of equipped weapons
		EquippedWeapons.Add(NewWeapon);

		// Attach new weapon to appropriate holster
		AttachWeaponToSocket(NewWeapon);
	}
}

void USoSPlayerInventory::AttachWeaponToSocket(ASoSBaseWeapon* Weapon, bool bDrawWeapon)
{
	if (bDrawWeapon)
	{
		Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandAttachPoint);
	}
	else
	{
		switch (Weapon->GetWeaponType())
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

void USoSPlayerInventory::HolsterUnholster()
{ 
	if (CurrentWeapon)
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
			PlayerCharacter->SetPlayerSpeed(CurrentWeapon->GetWeaponDrawnSpeedMultiplier());
			PlayerCharacter->SetPlayerMovementType(false, true);
		}
	} 
}

void USoSPlayerInventory::CycleWeapons(bool bNextWeapon)
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

/////////////////////////
/* Getters and Setters */
/////////////////////////

ASoSBaseWeapon* USoSPlayerInventory::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool USoSPlayerInventory::GetWeaponIsDrawn() const
{
	return bWeaponIsDrawn;
}

void USoSPlayerInventory::SetWeaponIsDrawn(bool WeaponIsDrawn)
{
	bWeaponIsDrawn = WeaponIsDrawn;
}
