// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerAbilityTarget.h"
#include "SoSAbilityBase.h"
#include "SoSCharacterBase.h"
#include "SoSPlayerCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


ASoSPlayerAbilityTarget::ASoSPlayerAbilityTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	TubeTargetShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TubeTargetShape"));
	TubeTargetShape->SetupAttachment(GetDecal());
	TubeTargetShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereTargetShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereTargetShape"));
	SphereTargetShape->SetupAttachment(GetDecal());
	SphereTargetShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	bSnapToGround = true;
	MaxTargetRange = 1500;
	TargetRadius = 250;
}


void ASoSPlayerAbilityTarget::BeginPlay()
{
	Deactivate();
}


void ASoSPlayerAbilityTarget::Activate(EAbilityTargetingShape TargetingShape)
{
	switch (TargetingShape)
	{
	case EAbilityTargetingShape::Tube:
		GetDecal()->SetVisibility(true);
		TubeTargetShape->SetVisibility(true);
		break;
	case EAbilityTargetingShape::Sphere:
		SphereTargetShape->SetVisibility(true);
		break;
	default:
		break;
	}

}


void ASoSPlayerAbilityTarget::Deactivate()
{
	GetDecal()->SetVisibility(false, true);
}


FVector ASoSPlayerAbilityTarget::GetTargetLocation()
{
	FHitResult AimHit = PlayerCharacter->GetAimHitResult();
	FHitResult TargetingHit;

	FVector StartLocation;
	FVector EndLocation;

	bInRange = (AimHit.Location - AimHit.TraceStart).Size() < MaxTargetRange;

	ASoSCharacterBase* HitCharacter = Cast<ASoSCharacterBase>(AimHit.Actor);
	if (HitCharacter != nullptr && bInRange)
	{
		if (bSnapToGround)
		{
			StartLocation = HitCharacter->GetActorLocation();
			EndLocation = StartLocation + FVector(0, 0, -10000);
			TargetingTrace(TargetingHit, StartLocation, EndLocation);
			GetDecal()->SetWorldLocation(TargetingHit.Location);
			return TargetingHit.Location;
		}

		GetDecal()->SetWorldLocation(HitCharacter->GetActorLocation());
		return HitCharacter->GetActorLocation();
	}

	float OffsetFromHit = AimHit.bBlockingHit ? 5 : 0;
	float TraceLength = FMath::Clamp((AimHit.Location - AimHit.TraceStart).Size() - OffsetFromHit, 0.0f, MaxTargetRange);
	StartLocation = AimHit.Location - AimHit.TraceStart;
	StartLocation.Normalize();
	StartLocation = AimHit.TraceStart + (StartLocation * TraceLength);
	EndLocation = StartLocation + FVector(0, 0, -10000);

	if (!bSnapToGround)
	{
		GetDecal()->SetWorldLocation(StartLocation);
		return StartLocation;
	}

	TargetingTrace(TargetingHit, StartLocation, EndLocation);
	
	FHitResult VisibilityTrace;
	StartLocation = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
	EndLocation = TargetingHit.Location - StartLocation;
	EndLocation.Normalize();
	EndLocation = StartLocation + EndLocation * ((TargetingHit.Location - StartLocation).Size() - 10);

	if (TargetingTrace(VisibilityTrace, StartLocation, EndLocation))
	{
		EndLocation = GetDecal()->GetComponentLocation() - StartLocation;
		EndLocation.Normalize();
		EndLocation = StartLocation + EndLocation * ((GetDecal()->GetComponentLocation() - StartLocation).Size() - 10);
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
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, PlayerCharacter);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		return true;
	}

	OutHit.Location = EndLocation;
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
	SetActorRelativeScale3D(FVector(TargetRadius));
}


void ASoSPlayerAbilityTarget::SetPlayerCharacter(ASoSPlayerCharacter* NewPlayer)
{
	PlayerCharacter = NewPlayer;
}
