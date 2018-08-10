// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoSASComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

protected:

	float HealthCurrentValue;

	float SpeedCurrentValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float HealthBaseValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float SpeedBaseValue;

public:

	void SetASAttribute(EASAttributeName AttributeToSet, float Value, float Duration);

	float GetASAttribute(EASAttributeName AttributeToGet) const;
	
};
