// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SoSGameModeBase.generated.h"

class USoSASComponent;

UCLASS()
class SEAOFSAND_API ASoSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASoSGameModeBase();

	virtual void Tick(float DeltaSeconds) override;

public:

	void AddASComponentToArray(USoSASComponent* ASComp);

	void RemoveASComponentToArray(USoSASComponent* ASComp);

protected:

	TArray<USoSASComponent*> ASComponentsToCheck;	

	void CheckASComponents();
	
};
