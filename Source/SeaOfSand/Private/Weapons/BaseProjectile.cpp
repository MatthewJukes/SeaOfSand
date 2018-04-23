// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Public/TimerManager.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup projectile
	ProjectileCapsule = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCapsule"));
	//ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle")); // TODO figure out attachment issues
	RootComponent = ProjectileCapsule;
	//ProjectileParticle->SetupAttachment(ProjectileCapsule);
	
	// Setup projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::BeginOverlap);

	// Default lifetime
	ProjectileLifetime = 2.f;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ABaseProjectile::DestroyProjectile, ProjectileLifetime, false);
}

void ABaseProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								   int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	//DestroyProjectile();
}

void ABaseProjectile::DestroyProjectile()
{
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	Destroy();
}

