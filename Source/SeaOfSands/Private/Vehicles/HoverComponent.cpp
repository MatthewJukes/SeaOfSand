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
	CompressionRatio = (HitLocation - GetComponentLocation()).Size() / TraceLength;
	FVector Force = HitNormal * FMath::Lerp(HoverForce, 0.f, CompressionRatio);
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
		HitNormal = GetUpVector();
	}
	
}

