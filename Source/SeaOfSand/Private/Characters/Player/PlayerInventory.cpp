// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerInventory.h"
#include "PlayerCharacter.h"
#include "BaseWeapon.h"
#include "Rifle.h"

// Sets default values for this component's properties
UPlayerInventory::UPlayerInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Socket names
	RightHandAttachPoint = TEXT("RightHandSocket");
	RifleHolsterAttachPoint = TEXT("RifleHolsterSocket");

	CurrentWeaponType = ECurrentWeaponType::Rifle; // TODO make a proper system for this
}


// Called when the game starts
void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();

	// Get the player character
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got Player"));
		SpawnWeapon();
	}
}

void UPlayerInventory::SpawnWeapon()
{
	FActorSpawnParameters SpawnParams;
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(RifleBlueprint, SpawnParams); // TODO make it handle multiple weapon types and add a selection sytem

	AttachWeaponToSocket();
}


void UPlayerInventory::AttachWeaponToSocket(bool bDrawWeapon)
{
	if (bDrawWeapon)
	{
		CurrentWeapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandAttachPoint);
	}
	else
	{
		switch (CurrentWeaponType)
		{
		case ECurrentWeaponType::Pistol:
			break;
		case ECurrentWeaponType::Rifle:
			CurrentWeapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleHolsterAttachPoint);
			break;
		case ECurrentWeaponType::Shotgun:
			break;
		default:
			break;
		}
	}
}

void UPlayerInventory::HolsterUnholster()
{ 
	if (CurrentWeapon && !PlayerCharacter->bIsAiming) // TODO refine and refactor this
	{
		if (bWeaponIsDrawn) // Holster weapon
		{
			AttachWeaponToSocket();
			bWeaponIsDrawn = false;
			PlayerCharacter->OffsetCamera(false);

			CurrentWeapon->InterruptReload();

			// Update character movement settings
			PlayerCharacter->SetPlayerSpeed(1.f);
			PlayerCharacter->SetPlayerMovementType(true, false);
		}
		else // Unholster weapon
		{
			AttachWeaponToSocket(true);
			bWeaponIsDrawn = true;
			PlayerCharacter->OffsetCamera(true);

			// Update character movement settings
			PlayerCharacter->SetPlayerSpeed(CurrentWeapon->WeaponDrawnSpeedMultiplier);
			PlayerCharacter->SetPlayerMovementType(false, true);
		}
	} 
}