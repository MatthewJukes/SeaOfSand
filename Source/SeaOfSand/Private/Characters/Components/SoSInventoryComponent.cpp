// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSInventoryComponent.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerHUD.h"
#include "SoSWeaponBase.h"
#include "SoSRangedWeapon.h"

// Sets default values for this component's properties
USoSInventoryComponent::USoSInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Socket names
	LeftHipAttachPoint = TEXT("LeftHipSocket");
	RightHandAttachPoint = TEXT("RightHandSocket");
	RightHipAttachPoint = TEXT("RightHipSocket");
}

// Called when the game starts
void USoSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get the player character
	PlayerCharacter = Cast<ASoSPlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got Player"));
		SpawnWeapon(RangedWeapon);

		CurrentWeapon = EquippedWeapons[0];
		CurrentWeaponArrayID = 0;
	}
}

void USoSInventoryComponent::SpawnWeapon(TSubclassOf<ASoSWeaponBase> WeaponToSpawn)
{
	// Spawn new weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASoSWeaponBase* NewWeapon = GetWorld()->SpawnActor<ASoSWeaponBase>(WeaponToSpawn, SpawnParams);
	
	if (NewWeapon)
	{
		// Add new weapon to array of equipped weapons
		EquippedWeapons.Add(NewWeapon);

		// Attach new weapon to appropriate holster
		AttachWeaponToSocket(NewWeapon);
	}
}

void USoSInventoryComponent::AttachWeaponToSocket(ASoSWeaponBase* Weapon, bool bDrawWeapon)
{
	if (bDrawWeapon)
	{
		Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandAttachPoint);
	}
	else
	{
		switch (Weapon->GetWeaponType())
		{
		case EWeaponType::Ranged:
			Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHipAttachPoint);
			break;
		case EWeaponType::Melee:
			Weapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHipAttachPoint);
			break;
		default:
			break;
		}
	}
}

void USoSInventoryComponent::HolsterUnholster()
{ 
	if (CurrentWeapon)
	{
		if (bWeaponIsDrawn) // Holster weapon
		{
			if (PlayerCharacter->bIsAiming) { PlayerCharacter->AimEnd(); }
			AttachWeaponToSocket(CurrentWeapon);
			bWeaponIsDrawn = false;
			PlayerCharacter->OffsetCamera(false);

			//CurrentWeapon->InterruptReload();

			// Update character movement settings
			PlayerCharacter->SetPlayerSpeed(1.f);
			PlayerCharacter->SetPlayerMovementType(true, false);

			// Toggle crosshair
			PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
		}
		else // Unholster weapon
		{
			AttachWeaponToSocket(CurrentWeapon, true);
			bWeaponIsDrawn = true;
			PlayerCharacter->OffsetCamera(true);

			// Update character movement settings
			//PlayerCharacter->SetPlayerSpeed(CurrentWeapon->GetWeaponDrawnSpeedMultiplier());
			PlayerCharacter->SetPlayerMovementType(false, true);

			// Toggle crosshair
			PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
		}
	} 
}

void USoSInventoryComponent::CycleWeapons(bool bNextWeapon)
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

ASoSWeaponBase* USoSInventoryComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool USoSInventoryComponent::GetWeaponIsDrawn() const
{
	return bWeaponIsDrawn;
}

void USoSInventoryComponent::SetWeaponIsDrawn(bool WeaponIsDrawn)
{
	bWeaponIsDrawn = WeaponIsDrawn;
}
