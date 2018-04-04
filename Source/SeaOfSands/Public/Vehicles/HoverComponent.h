// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HoverComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSANDS_API UHoverComponent : public USceneComponent
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

private:
	
	UStaticMeshComponent* ParentMesh;

	void AddForceToParent();

	void HoverTrace();

	UPROPERTY(EditDefaultsOnly)
	float TraceLength = 60.f;

	UPROPERTY(EditDefaultsOnly)
	float HoverForce = 400000.f;

	FVector HitLocation;
	FVector HitNormal;
};
