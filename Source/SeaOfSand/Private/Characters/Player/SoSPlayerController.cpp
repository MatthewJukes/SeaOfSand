// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerInventory.h"
#include "PlayerHUD.h"
#include "BaseWeapon.h"
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
	PlayerHUD = Cast<APlayerHUD>(GetHUD());
}

void ASoSPlayerController::UpdateCurrentPawn()
{
	CurrentPlayerPawn = GetPawn(); // Set current pawn
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
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		if (!PlayerCharacter->PlayerInventory->bWeaponIsDrawn) // Draw weapon is not drawn already
		{
			PlayerCharacter->PlayerInventory->HolsterUnholster();
		}
		else
		{
			if (PlayerCharacter->bIsSprinting) { PlayerCharacter->SprintEnd(); }
			PlayerCharacter->PlayerInventory->CurrentWeapon->StartFiring();
		}		
	}
}

void ASoSPlayerController::StopFiring()
{
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		PlayerCharacter->PlayerInventory->CurrentWeapon->StopFiring();
	}
}

void ASoSPlayerController::HolsterUnholster()
{
	if (PlayerCharacter->PlayerInventory)
	{
		PlayerCharacter->PlayerInventory->HolsterUnholster();
	}
}

void ASoSPlayerController::Reload()
{
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		PlayerCharacter->PlayerInventory->CurrentWeapon->StartReload();
	}
}

void ASoSPlayerController::NextWeapon()
{
	PlayerCharacter->PlayerInventory->CycleWeapons();
}

void ASoSPlayerController::PrevWeapon()
{
	PlayerCharacter->PlayerInventory->CycleWeapons(false);
}

FVector ASoSPlayerController::GetCrosshairHitLocation() const
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
		return GetLookVectorHitLocation(LookDirection);
	}
	return FVector(0.f,0.f,0.f);
}

bool ASoSPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(ScreenLocation.X,	ScreenLocation.Y, CameraWorldLocation, LookDirection);
}

FVector ASoSPlayerController::GetLookVectorHitLocation(FVector LookDirection) const
{
	const FName TraceTag("CrosshairTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;
	FVector StartLocation = PlayerCameraManager->GetCameraLocation();
	FVector EndLocation = StartLocation + (LookDirection * CrosshairTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		// Set hit location
		return RV_Hit.Location;
	}
	return EndLocation; // return end location if nothing hit
}

FRotator ASoSPlayerController::GetAimOffsets() const
{
	const FVector AimDirWS = CurrentPlayerPawn->GetBaseAimRotation().Vector();
	const FVector AimDirLS = CurrentPlayerPawn->ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}