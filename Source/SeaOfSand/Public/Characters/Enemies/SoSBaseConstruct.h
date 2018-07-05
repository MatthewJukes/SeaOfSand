// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoSBaseConstruct.generated.h"

class USoSHealthComponent;

UCLASS()
class SEAOFSAND_API ASoSBaseConstruct : public ACharacter
{
	GENERATED_BODY()
	
public:

	ASoSBaseConstruct();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USoSHealthComponent* HealthComp;


public:

protected:

public:

	
};
