// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HoverComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API UHoverComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHoverComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintReadOnly)
	float CompressionRatio;

	FVector HitLocation;
	FVector HitNormal;

protected:

	UPROPERTY(EditDefaultsOnly)
	float TraceLength = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float ThrustLength = 130.f;

	UPROPERTY(EditDefaultsOnly)
	float HoverForce = 600000.f;

private:
	
	UStaticMeshComponent* ParentMesh;

	void AddForceToParent();

	void HoverTrace();
};
