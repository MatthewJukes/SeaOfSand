// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "BaseWeapon.h"


void ABasePlayerController::SetupInputComponent()
{
	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
		InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
		UE_LOG(LogTemp, Warning, TEXT("Bound Input"));
	}
}

void ABasePlayerController::UpdateCurrentWeapon(ABaseWeapon * NewWeapon)
{
	CurrentWeapon = Cast<ABaseWeapon>(NewWeapon);
}

void ABasePlayerController::StartFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void ABasePlayerController::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
} 
