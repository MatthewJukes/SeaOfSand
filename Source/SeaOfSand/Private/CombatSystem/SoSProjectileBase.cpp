// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ASoSProjectileBase::ASoSProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(("ProjectileMovement"));
}


AActor* ASoSProjectileBase::GetProjectileSource() const
{
	return ProjectileSource;
}


void ASoSProjectileBase::SetProjectileSource(AActor* Source)
{
	ProjectileSource = Source;
}

