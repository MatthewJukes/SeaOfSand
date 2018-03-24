// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "BasePlayerController.h"
#include "Engine/World.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::StartFiring(){}
void ABaseWeapon::StopFiring(){}

bool ABaseWeapon::CheckIfWeaponCanFire(float ElaspedTime, float LastElaspedTime, float FireRate) const
{
	return true;
}

bool ABaseWeapon::WeaponTrace(FVector MuzzleLocation, float MaxRange, float BulletSpread) const
{
	const FName TraceTag("WeaponTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;

	FVector StartLocation = MuzzleLocation;

	FVector CrosshairHitLocation;
	PlayerController->GetCrosshairHitLocation(CrosshairHitLocation);

	FVector AimDirection = CrosshairHitLocation - StartLocation;
	AimDirection.Normalize();
	FVector EndLocation = StartLocation + (AimDirection * MaxRange);
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		return true;
	}
	return false; // Line-trace didn't hit anything
}



