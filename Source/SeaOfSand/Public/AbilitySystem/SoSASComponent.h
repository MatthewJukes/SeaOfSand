// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"

UENUM(BlueprintType)
enum class EAttributes : uint8
{
	health,
	speed,
};

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



		
	
};
