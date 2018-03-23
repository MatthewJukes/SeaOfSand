// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "BaseWeapon.h"

void ABasePlayerController::SetupInputComponent()
{
	InputComponent->BindAction("Fire", IE_Pressed, this, &ABasePlayerController::StartFiring);
	InputComponent->BindAction("Fire", IE_Released, this, &ABasePlayerController::StopFiring);
}

//void ABasePlayerController::UpdateCurrentWeapon(ABaseWeapon * NewWeapon)
//{
	//CurrentWeapon = Cast<ABaseWeapon>(NewWeapon);
//}

void ABasePlayerController::StartFiring()
{
}

void ABasePlayerController::StopFiring()
{
}
