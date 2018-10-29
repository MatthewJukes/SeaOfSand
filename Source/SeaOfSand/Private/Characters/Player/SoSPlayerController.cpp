// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "SoSPlayerHUD.h"
#include "SoSRangedWeapon.h"
#include "SeaOfSand.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"

void ASoSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASoSPlayerController::StartFiring);
		InputComponent->BindAction("Fire", IE_Released, this, &ASoSPlayerController::StopFiring);
		InputComponent->BindAction("Holster", IE_Pressed, this, &ASoSPlayerController::HolsterUnholster);
		InputComponent->BindAction("Reload", IE_Pressed, this, &ASoSPlayerController::Reload);
		InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ASoSPlayerController::NextWeapon);
		InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ASoSPlayerController::PrevWeapon);
	}	
}

// Called when the game starts or when spawned
void ASoSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UpdateCurrentPawn();
	PlayerCharacter = Cast<ASoSPlayerCharacter>(CurrentPlayerPawn);
	PlayerInventory = PlayerCharacter->GetCharacterInventory();
	PlayerHUD = Cast<ASoSPlayerHUD>(GetHUD());
}

void ASoSPlayerController::UpdateCurrentPawn()
{
	CurrentPlayerPawn = GetPawn();
}

void ASoSPlayerController::ToggleVehicleHud()
{
	if (PlayerHUD)
	{
		PlayerHUD->ToggleVehicleHud();
	}
}

void ASoSPlayerController::StartFiring()
{
	if (PlayerInventory)
	{
		if (PlayerInventory->GetCurrentWeapon()->GetWeaponState() == EWeaponState::Holstered) // Draw weapon if not drawn already
		{
			PlayerInventory->HolsterUnholster();
		}
		else
		{
			PlayerCharacter->SprintEnd();
			PlayerInventory->GetCurrentWeapon()->StartAttack();
		}		
	}
}

void ASoSPlayerController::StopFiring()
{
	if (PlayerInventory)
	{
		PlayerInventory->GetCurrentWeapon()->EndAttack();
	}
}

void ASoSPlayerController::HolsterUnholster()
{
	if (PlayerInventory)
	{
		PlayerInventory->HolsterUnholster();
	}
}

void ASoSPlayerController::Reload()
{
	if (PlayerInventory)
	{
		//PlayerInventory->GetCurrentWeapon()->StartReload();
	}
}

void ASoSPlayerController::NextWeapon()
{
	PlayerInventory->CycleWeapons();
}

void ASoSPlayerController::PrevWeapon()
{
	PlayerInventory->CycleWeapons(false);
}

FVector ASoSPlayerController::GetCrosshairHitLocation(bool bOffsetFromCamera, FVector OffsetTarget) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewPortSizeY;
	GetViewportSize(ViewportSizeX, ViewPortSizeY);
	FVector2D ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewPortSizeY * CrosshairYLocation);

	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along LookDirection, and see what we hit (up to max range)
		return GetLookVectorHitLocation(LookDirection, bOffsetFromCamera, OffsetTarget);
	}
	return FVector(0.f,0.f,0.f);
}

bool ASoSPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(ScreenLocation.X,	ScreenLocation.Y, CameraWorldLocation, LookDirection);
}

FVector ASoSPlayerController::GetLookVectorHitLocation(FVector LookDirection, bool bOffsetFromCamera, FVector OffsetTarget) const
{
	const FName TraceTag("CrosshairTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	FHitResult Hit;

	float Offset = 0.0f;

	if (bOffsetFromCamera)
	{
		Offset = FVector::Dist(PlayerCameraManager->GetCameraLocation(), OffsetTarget);
	}

	FVector StartLocation = PlayerCameraManager->GetCameraLocation() + (LookDirection * Offset);
	FVector EndLocation = StartLocation + (LookDirection * CrosshairTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, COLLISION_CROSSHAIR, TraceParams))
	{
		// Set hit location
		return Hit.Location;
	}
	return EndLocation; // return end location if nothing hit
}

ASoSPlayerHUD * ASoSPlayerController::GetPlayerHUD() const
{
	return PlayerHUD;
}

FRotator ASoSPlayerController::GetAimOffsets() const
{
	const FVector AimDirWS = CurrentPlayerPawn->GetBaseAimRotation().Vector();
	const FVector AimDirLS = CurrentPlayerPawn->ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}