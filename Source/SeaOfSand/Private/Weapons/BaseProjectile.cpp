// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Public/TimerManager.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Default lifetime
	ProjectileLifetime = 2.f;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ABaseProjectile::DestroyProjectile, ProjectileLifetime, false);
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseProjectile::DestroyProjectile()
{
	Destroy();
}

