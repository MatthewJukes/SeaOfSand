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
#include "SoSCharacterBase.h"
#include "SoSAIDecisionData.h"
#include "SoSAICharacterBase.generated.h"


class USoSAIDecision;
class UAIPerceptionComponent;

USTRUCT(BlueprintType)
struct FAbilityDecisionPair
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TSubclassOf<USoSAbilityBase> Ability;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FAIAbilityDecision AbilityDecision;
};


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

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UAIPerceptionComponent* PerceptionComp;

public:

	void CreateDecisionObjects();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UtilityAI")
	TArray<FAbilityDecisionPair> AbilityDecisionPairs;

	UPROPERTY(EditDefaultsOnly, Category = "UtilityAI")
	TArray<FAIActionDecision> ActionDecisions;

	UPROPERTY()
	TArray<USoSAIDecision*> DecisionObjects;

public:

	TArray<USoSAIDecision*>& GetDecisions();
};
