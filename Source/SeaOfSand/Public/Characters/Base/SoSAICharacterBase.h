/************************************************************************************************************************************
*
*	SoSAICharacterBase.h
*	AI character base
*
*	Written by Matthew Jukes (20/11/2018)
*	Owned by Matthew Jukes
*
****/

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/SoSCharacterBase.h"
#include "SoSAICharacterBase.generated.h"


class USoSAIDecision;


UCLASS()
class SEAOFSAND_API ASoSAICharacterBase : public ASoSCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASoSAICharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void CreateDecisionObjects();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UtilityAI")
	TArray<TSubclassOf<USoSAIDecision>> BP_Decisions;

	TArray<USoSAIDecision*> Decisions;

public:

	TArray<USoSAIDecision*>& GetDecisions();
};
