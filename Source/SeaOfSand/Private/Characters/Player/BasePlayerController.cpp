// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "PlayerInventory.h"
#include "PlayerHUD.h"
#include "BaseWeapon.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
		InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
		InputComponent->BindAction("Use", IE_Released, this, &ABasePlayerController::Interact);
		InputComponent->BindAction("Holster", IE_Pressed, this, &ABasePlayerController::HolsterUnholster);
		InputComponent->BindAction("Reload", IE_Pressed, this, &ABasePlayerController::Reload);
	}	
}

// Called when the game starts or when spawned
void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	UpdateCurrentPawn();
	PlayerCharacter = Cast<APlayerCharacter>(CurrentPlayerPawn);
	PlayerHUD = Cast<APlayerHUD>(GetHUD());
}

void ABasePlayerController::UpdateCurrentPawn()
{
	CurrentPlayerPawn = GetPawn(); // Set current pawn
	if (CurrentPlayerPawn) { InputInterface = Cast<IPlayerInputsInterface>(CurrentPlayerPawn); } // Set input interface //TODO do I need to use an interface?
}

void ABasePlayerController::ToggleVehicleHud()
{
	if (PlayerHUD)
	{
		PlayerHUD->ToggleVehicleHud();
	}
}

void ABasePlayerController::StartFiring()
{	 
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		if (!PlayerCharacter->bWeaponIsDrawn) // Draw weapon is not drawn already
		{
			PlayerCharacter->PlayerInventory->HolsterUnholster();
		}
		else if (PlayerCharacter->bCanFire)
		{
			if (PlayerCharacter->bIsSprinting) { PlayerCharacter->SprintEnd(); }
			PlayerCharacter->PlayerInventory->CurrentWeapon->StartFiring();
		}		
	}
}

void ABasePlayerController::StopFiring()
{
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		PlayerCharacter->PlayerInventory->CurrentWeapon->StopFiring();
	}
}

void ABasePlayerController::Interact()
{	
	if (InputInterface)
	{		
		InputInterface->Execute_Interact(CurrentPlayerPawn);
	}
}

void ABasePlayerController::HolsterUnholster()
{
	if (PlayerCharacter->PlayerInventory)
	{
		PlayerCharacter->PlayerInventory->HolsterUnholster();
	}
}

void ABasePlayerController::Reload()
{
	if (PlayerCharacter->PlayerInventory->CurrentWeapon)
	{
		PlayerCharacter->PlayerInventory->CurrentWeapon->StartReload();
	}
}

FVector ABasePlayerController::GetCrosshairHitLocation() const
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

bool ABasePlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(ScreenLocation.X,	ScreenLocation.Y, CameraWorldLocation, LookDirection);
}

FVector ABasePlayerController::GetLookVectorHitLocation(FVector LookDirection) const
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

FRotator ABasePlayerController::GetAimOffsets() const
{
	const FVector AimDirWS = CurrentPlayerPawn->GetBaseAimRotation().Vector();
	const FVector AimDirLS = CurrentPlayerPawn->ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}