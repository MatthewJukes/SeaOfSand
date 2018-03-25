// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "BaseWeapon.h"
#include "Engine/World.h"

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
		InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
	}
}

// Called when the game starts or when spawned
void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<APlayerCharacter>(GetCharacter());
}

void ABasePlayerController::UpdateCurrentWeapon(ABaseWeapon * NewWeapon)
{
	CurrentWeapon = Cast<ABaseWeapon>(NewWeapon);
}

void ABasePlayerController::StartFiring()
{
	if (CurrentWeapon && Player)
	{
		if (Player->bCanFire)
		{
			CurrentWeapon->StartFiring();
		}		
	}
}

void ABasePlayerController::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
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
	FVector EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		// Set hit location
		return RV_Hit.Location;
	}
	return EndLocation; // return end location if nothing hit
}