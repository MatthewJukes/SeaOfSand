// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSCharacterBase.h"
#include "SeaOfSand.h"
#include "SoSCombatComponent.h"
#include "SoSInventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASoSCharacterBase::ASoSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_ABILITY, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Ignore);	

	// Setup combat component
	CombatComp = CreateDefaultSubobject<USoSCombatComponent>(TEXT("CombatComp"));
	
	// Setup inventory
	InventoryComp = CreateDefaultSubobject<USoSInventoryComponent>(TEXT("PlayerInventory"));

	// Configure character movement 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = CombatComp->GetAttributeTotalValue(EAttributeName::Speed);
	GetCharacterMovement()->AirControl = 0.2f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}


void ASoSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASoSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/////////////////////////
/* Getters and Setters */
/////////////////////////


USoSInventoryComponent * ASoSCharacterBase::GetCharacterInventory() const
{
	return InventoryComp;
}


USoSCombatComponent* ASoSCharacterBase::GetCharacterCombatComponent() const
{
	return CombatComp;
}


FHitResult ASoSCharacterBase::GetAimHitResult() const
{
	return AimHitResult;
}


FVector ASoSCharacterBase::GetTargetedLocation() const
{
	return TargetedLocation;
}

void ASoSCharacterBase::SetCharacterMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = bOrientRotationToMovement;
		CharacterMovement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
	}
}


FGenericTeamId ASoSCharacterBase::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)CombatComp->GetTeam());
}

