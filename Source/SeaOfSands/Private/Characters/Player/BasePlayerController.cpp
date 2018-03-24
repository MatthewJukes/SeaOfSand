// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "BaseWeapon.h"

void ABasePlayerController::SetupInputComponent()
{
	InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
	InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
}

<<<<<<< HEAD
void ABasePlayerController::SetupInputComponent()
{
	//InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
	//InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
}
=======
//void ABasePlayerController::UpdateCurrentWeapon(ABaseWeapon * NewWeapon)
//{
	//CurrentWeapon = Cast<ABaseWeapon>(NewWeapon);
//}
>>>>>>> GitHub/master

void ABasePlayerController::StartFiring()
{
}

<<<<<<< HEAD
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
=======
void ABasePlayerController::StopFiring()
{
}
>>>>>>> GitHub/master
