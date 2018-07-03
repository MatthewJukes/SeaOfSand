// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/SoSBaseEnemy.h"
#include "SoSBaseConstruct.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS()
class SEAOFSAND_API ASoSBaseConstruct : public ASoSBaseEnemy
{
	GENERATED_BODY()
	
public:

	ASoSBaseConstruct();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UCapsuleComponent* CapsuleComp;

	//UPROPERTY(VisibleAnywhere, Category = "Component")
	//UCharacterMovementComponent* MovementComp;

public:

protected:

public:

	
};
