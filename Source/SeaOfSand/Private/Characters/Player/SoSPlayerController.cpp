// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "SoSPlayerHUD.h"
#include "SoSRangedWeapon.h"
#include "SeaOfSand.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"

ASoSPlayerController::ASoSPlayerController()
{
	CrosshairXLocation = 0.5f;
	CrosshairYLocation = 0.5f;
	CrosshairTraceRange = 10000;
}

void ASoSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Primary", IE_Pressed, this, &ASoSPlayerController::PrimaryAttackStart);
		InputComponent->BindAction("Primary", IE_Released, this, &ASoSPlayerController::PrimaryAttackEnd);
		InputComponent->BindAction("Alternate", IE_Pressed, this, &ASoSPlayerController::AlternateAttackStart);
		InputComponent->BindAction("Alternate", IE_Released, this, &ASoSPlayerController::AlternateAttackEnd);
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


void ASoSPlayerController::PrimaryAttackStart()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->PrimaryAttackStart();
	}
}


void ASoSPlayerController::PrimaryAttackEnd()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->PrimaryAttackEnd();
	}
}


void ASoSPlayerController::AlternateAttackStart()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->AlternateAttackStart();
	}
}


void ASoSPlayerController::AlternateAttackEnd()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->AlternateAttackEnd();
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


bool ASoSPlayerController::GetCrosshairHitResult(FHitResult &OutHitResult, bool bOffsetFromCamera, FVector OffsetTarget) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewPortSizeY;
	GetViewportSize(ViewportSizeX, ViewPortSizeY);
	FVector2D ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewPortSizeY * CrosshairYLocation);

	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	GetLookDirection(ScreenLocation, LookDirection);

	// Line-trace along LookDirection, and see what we hit (up to max range)
	return GetLookVectorHitResult(OutHitResult, LookDirection, bOffsetFromCamera, OffsetTarget);
}


bool ASoSPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(ScreenLocation.X,	ScreenLocation.Y, CameraWorldLocation, LookDirection);
}


bool ASoSPlayerController::GetLookVectorHitResult(FHitResult &OutHitResult, FVector LookDirection, bool bOffsetFromCamera, FVector OffsetTarget) const
{
	const FName TraceTag("CrosshairTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	float Offset = 0.0f;

	if (bOffsetFromCamera)
	{
		Offset = FVector::Dist(PlayerCameraManager->GetCameraLocation(), OffsetTarget);
	}

	FVector StartLocation = PlayerCameraManager->GetCameraLocation() + (LookDirection * Offset);
	FVector EndLocation = StartLocation + (LookDirection * CrosshairTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(OutHitResult, StartLocation, EndLocation, COLLISION_CROSSHAIR, TraceParams))
	{
		return true;
	}

	OutHitResult.Location = EndLocation;
	return false;
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