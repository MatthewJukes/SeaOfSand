// Fill out your copyright notice in the Description page of Project Settings.

#include "HoverComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Math/Vector.h"
#include "Math/UnrealMathVectorCommon.h"


// Sets default values for this component's properties
UHoverComponent::UHoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHoverComponent::BeginPlay()
{
	Super::BeginPlay();

	ParentMesh = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());	
	if (ParentMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent mesh is %s"), *GetNameSafe(ParentMesh))
	}
}


// Called every frame
void UHoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AddForceToParent();
}

void UHoverComponent::AddForceToParent()
{
	HoverTrace();

	// Increase hover force magnitude based on distance between component and ground
	float ThrustRatio = FMath::Clamp((HitLocation - GetComponentLocation()).Size() / CalculateThrustLength(), 0.f, 1.f);
	ShortCompressionRatio = FMath::Clamp((HitLocation - GetComponentLocation()).Size() / (CalculateThrustLength() + 30.f), 0.f, 1.f);
	LongCompressionRatio = FMath::Clamp((HitLocation - GetComponentLocation()).Size() / TraceLength, 0.f, 1.f);
	FVector Force = HitNormal * FMath::Lerp(RepulseHoverForce, 0.f, ThrustRatio); // Repulsion force
	Force += HitNormal * FMath::Lerp(-AttractHoverForce*0.1f, -AttractHoverForce, FMath::Clamp(LongCompressionRatio - (1.f - ThrustRatio), 0.f, 1.f));
	ParentMesh->AddForceAtLocation(Force, GetComponentLocation());
}

void UHoverComponent::HoverTrace()
{
	const FName TraceTag("HoverTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;
	FVector StartLocation = GetComponentLocation();
	FVector EndLocation = StartLocation + (GetUpVector() * -TraceLength);

	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		HitLocation = RV_Hit.Location;
		HitNormal = RV_Hit.Normal;
	}
	else
	{
		HitLocation = EndLocation;
		HitNormal = FVector(0.f,0.f,1.f);
	}
	
}

float UHoverComponent::CalculateThrustLength()
{
	float ThrustLength = FMath::Lerp(FMath::Clamp(FVector::Dist(GetComponentLocation(), HitLocation), ThrustRange.X, ThrustRange.Y), BaseThrustLength, 0.6f);
	return ThrustLength;
}

