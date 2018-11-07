// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerAbilityTarget.h"
#include "SoSCharacterBase.h"
#include "SoSPlayerCharacter.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"


ASoSPlayerAbilityTarget::ASoSPlayerAbilityTarget()
{
	bSnapToGround = true;
	MaxTargetRange = 1500;
	TargetRadius = 250;
}


void ASoSPlayerAbilityTarget::BeginPlay()
{
	GetDecal()->Deactivate();
}


void ASoSPlayerAbilityTarget::Activate()
{
	GetDecal()->Activate();
}


void ASoSPlayerAbilityTarget::Deactivate()
{
	GetDecal()->Deactivate();
}


FVector ASoSPlayerAbilityTarget::GetTargetLocation()
{
	FHitResult AimHit = PlayerCharacter->GetAimHitResult();
	FHitResult TargetingHit;

	bInRange = (AimHit.Location - AimHit.TraceStart).Size() >= MaxTargetRange;

	ASoSCharacterBase* HitCharacter = Cast<ASoSCharacterBase>(AimHit.Actor);
	if (HitCharacter != nullptr && bInRange)
	{
		TargetingTrace(TargetingHit, HitCharacter->GetActorLocation(), HitCharacter->GetActorLocation() - FVector(0, 0, -10000));
		GetDecal()->SetWorldLocation(TargetingHit.Location);
		return TargetingHit.Location;
	}

	FVector StartLocation;
	FVector EndLocation;

	float OffsetFromHit = AimHit.bBlockingHit ? 5 : 0;
	float TraceLength = FMath::Clamp((AimHit.Location - AimHit.TraceStart).Size() - OffsetFromHit, 0.0f, MaxTargetRange);
	StartLocation = AimHit.TraceStart + ((AimHit.Location - AimHit.TraceStart).Normalize() * TraceLength);
	EndLocation = StartLocation = FVector(0, 0, -10000);

	TargetingTrace(TargetingHit, StartLocation, EndLocation);

	FHitResult VisibilityTrace;
	StartLocation = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
	EndLocation = StartLocation + (TargetingHit.Location - StartLocation).Normalize() * ((TargetingHit.Location - StartLocation).Size() - 10);

	if (TargetingTrace(VisibilityTrace, StartLocation, EndLocation))
	{
		EndLocation = StartLocation + (GetDecal()->GetComponentLocation() - StartLocation).Normalize() * ((GetDecal()->GetComponentLocation() - StartLocation).Size() - 10);
		if (TargetingTrace(VisibilityTrace, StartLocation, EndLocation))
		{
			GetDecal()->SetWorldLocation(VisibilityTrace.Location);
			return VisibilityTrace.Location;
		}

		return GetDecal()->GetComponentLocation();
	}

	GetDecal()->SetWorldLocation(TargetingHit.Location);
	return TargetingHit.Location;
}


bool ASoSPlayerAbilityTarget::TargetingTrace(FHitResult &OutHit, const FVector &StartLocation, const FVector &EndLocation)
{
	const FName TraceTag("TargetingTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, PlayerCharacter);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		return true;
	}
	return false; // Line-trace didn't hit anything
}

void ASoSPlayerAbilityTarget::SetSnapToGround(bool bNewBool)
{
	bSnapToGround = bNewBool;
}


void ASoSPlayerAbilityTarget::SetMaxTargetRange(float Value)
{
	MaxTargetRange = Value;
}


void ASoSPlayerAbilityTarget::SetTargetRadius(float Value)
{
	TargetRadius = Value;
}


void ASoSPlayerAbilityTarget::SetPlayerCharacter(ASoSPlayerCharacter* NewPlayer)
{
	PlayerCharacter = NewPlayer;
}
