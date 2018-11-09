// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "SoSPlayerAbilityTarget.generated.h"

class ASoSPlayerCharacter;
enum class EAbilityTargetingShape : uint8;

UCLASS()
class SEAOFSAND_API ASoSPlayerAbilityTarget : public ADecalActor
{
	GENERATED_BODY()

public:

	ASoSPlayerAbilityTarget();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* TubeTargetShape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* SphereTargetShape;

public:

	void Activate(EAbilityTargetingShape TargetingShape);

	void Deactivate();

	FVector GetTargetLocation();

protected:

	bool bInRange;

	bool bTargetIsCharacter;

	bool bSnapToGround;

	ASoSPlayerCharacter* PlayerCharacter;

	float MaxTargetRange;

	float TargetRadius;

	bool TargetingTrace(FHitResult &OutHit, const FVector &StartLocation, const FVector &EndLocation);

public:
	
	void SetSnapToGround(bool bNewBool);

	void SetMaxTargetRange(float Value);

	void SetTargetRadius(float Value);
	
	void SetPlayerCharacter(ASoSPlayerCharacter* NewPlayer);
};
