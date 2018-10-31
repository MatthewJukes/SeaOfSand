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


void ASoSProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	//ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
}

AActor* ASoSProjectileBase::GetProjectileSource() const
{
	return ProjectileSource;
}


void ASoSProjectileBase::SetProjectileSource(AActor* Source)
{
	ProjectileSource = Source;
}


void ASoSProjectileBase::SetProjectileDamage(float Value)
{
	ProjectileDamage = Value;
}


void ASoSProjectileBase::SetProjectileSpeed(float Value)
{
	ProjectileSpeed = Value;
}

