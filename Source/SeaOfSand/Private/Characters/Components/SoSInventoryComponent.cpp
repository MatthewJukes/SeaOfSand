// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSInventoryComponent.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSASComponent.h"
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
		SpawnWeapon(MeleeWeapon);

		CurrentWeapon = EquippedWeapons[0];
		CurrentWeaponArrayID = 0;

		PlayerCharacter->GetPlayerASComponent()->SetOwnerWeaponMesh(CurrentWeapon->GetWeaponMesh());
		PlayerCharacter->GetPlayerASComponent()->SetWeaponProjectileOriginSocketName(CurrentWeapon->GetProjectileOriginSocketName());
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

void USoSInventoryComponent::HolsterUnholster(bool bQuickSwitch) // TODO less spaghetti
{ 
	if (CurrentWeapon == nullptr)
	{
		return;
	}

	if (CurrentWeapon->GetWeaponState() == EWeaponState::Holstered) // draw weapon
	{
		if (!bQuickSwitch)
		{
			if (!PlayerCharacter->UseAbility(CurrentWeapon->GetWeaponAbilities().AbilityWeaponDrawInstance))
			{
				return;
			}
		}
		
		AttachWeaponToSocket(CurrentWeapon, true);
		CurrentWeapon->SetWeaponState(EWeaponState::Idle);

		if (CurrentWeapon->GetWeaponType() == EWeaponType::Ranged)
		{
			PlayerCharacter->OffsetCamera(true);
			PlayerCharacter->SetPlayerMovementType(false, true);
			PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
		}
	}
	else // holster weapon
	{
		if (!bQuickSwitch)
		{
			if (!PlayerCharacter->UseAbility(CurrentWeapon->GetWeaponAbilities().AbilityWeaponHolsterInstance))
			{
				return;
			}
		}

		PlayerCharacter->AimEnd();
		AttachWeaponToSocket(CurrentWeapon);
		CurrentWeapon->SetWeaponState(EWeaponState::Holstered);

		if (CurrentWeapon->GetWeaponType() == EWeaponType::Ranged)
		{
			//CurrentWeapon->InterruptReload();
			PlayerCharacter->OffsetCamera(false);
			PlayerCharacter->SetPlayerMovementType(true, false);
			PlayerCharacter->GetPlayerController()->GetPlayerHUD()->ToggleCrosshair();
		}
	}
	
}

void USoSInventoryComponent::CycleWeapons(bool bNextWeapon)
{
	if (PlayerCharacter == nullptr)
	{
		return;
	}

	if (PlayerCharacter->GetPlayerASComponent()->GetASOwnerState() == EASOwnerState::PerformingAction)
	{
		return;
	}

	bool bWeaponWasDrawn = CurrentWeapon->GetWeaponState() != EWeaponState::Holstered;
	if (bWeaponWasDrawn) { HolsterUnholster(true); }
	
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

	PlayerCharacter->GetPlayerASComponent()->SetOwnerWeaponMesh(CurrentWeapon->GetWeaponMesh());
	PlayerCharacter->GetPlayerASComponent()->SetWeaponProjectileOriginSocketName(CurrentWeapon->GetProjectileOriginSocketName());

	if (bWeaponWasDrawn) { HolsterUnholster(true); }
}

/////////////////////////
/* Getters and Setters */
/////////////////////////

ASoSWeaponBase* USoSInventoryComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}
