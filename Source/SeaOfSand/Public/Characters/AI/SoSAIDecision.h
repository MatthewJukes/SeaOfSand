/************************************************************************************************************************************
*
*	SoSAIDecision.h
*	Utility-theory AI decision
*
*	Written by Matthew Jukes (21/11/2018)
*	Owned by Matthew Jukes
*
****/


#pragma once

#include "CoreMinimal.h"
#include "SoSAIDecisionData.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "SoSAIDecision.generated.h"


UCLASS(BlueprintType, Blueprintable)
class SEAOFSAND_API USoSAIDecision : public UObject
{
	GENERATED_BODY()
	
public:
	USoSAIDecision();

protected:
	
public:

	float ScoreDecision();

protected:

	FDecisionData DecisionData;

public:

	EDecisionAction GetAction() const;

	FDecisionContext& GetDecisionContext();

	TArray<FDataTableRowHandle>& GetConsiderations();
};
