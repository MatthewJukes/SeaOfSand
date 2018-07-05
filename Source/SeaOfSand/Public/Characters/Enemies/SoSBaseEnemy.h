// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SoSBaseEnemy.generated.h"


UCLASS()
class SEAOFSAND_API ASoSBaseEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASoSBaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
	
};
