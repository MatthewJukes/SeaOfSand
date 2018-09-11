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
		if (CurrentWeapon->GetWeaponState() == EWeaponState::Holstered) // Unholster weapon
		{
			if (PlayerCharacter->UseAbility(CurrentWeapon->GetWeaponAbilities().AbilityWeaponDraw))
			{
				AttachWeaponToSocket(CurrentWeapon, true);
				CurrentWeapon->SetWeaponState(EWeaponState::Idle);
				PlayerCharacter->OffsetCamera(true);

				// Update character movement settings
				PlayerCharacter->SetPlayerMovementType(false, true);

				// Toggle crosshair
				PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
			}			
		}
		else // holster weapon
		{
			if (PlayerCharacter->UseAbility(CurrentWeapon->GetWeaponAbilities().AbilityWeaponHolster))
			{
				PlayerCharacter->AimEnd();
				AttachWeaponToSocket(CurrentWeapon);
				CurrentWeapon->SetWeaponState(EWeaponState::Holstered);
				PlayerCharacter->OffsetCamera(false);

				//CurrentWeapon->InterruptReload();

				// Update character movement settings
				PlayerCharacter->SetPlayerMovementType(true, false);

				// Toggle crosshair
				PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
			}
		}
	} 
}

void USoSInventoryComponent::CycleWeapons(bool bNextWeapon)
{
	bool bWeaponWasDrawn = CurrentWeapon->GetWeaponState() != EWeaponState::Holstered;
	if (CurrentWeapon->GetWeaponState() == EWeaponState::Holstered) { HolsterUnholster(); }
	
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
