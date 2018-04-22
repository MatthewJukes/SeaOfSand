// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class SEAOFSAND_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	UPROPERTY(VisibleAnywhere, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ProjectileCapsule;

	//UPROPERTY(VisibleAnywhere)
	//UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(VisibleAnywhere, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileLifetime;

private:

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void DestroyProjectile();

	FTimerHandle LifetimeTimerHandle;	

public:
	FORCEINLINE class USphereComponent* GetProjectileCapsule() const { return ProjectileCapsule; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
