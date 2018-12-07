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


class ASoSAICharacterBase;


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

	// Is this a ability decision? otherwise an action decision
	bool bAbilityDecision;

	UPROPERTY()
	FActionDecisionData ActionDecisionData;

	UPROPERTY()
	FAbilityDecisionData AbilityDecisionData;

public:

	bool GetIsAbilityDecision() const;

	EDecisionAction GetAction() const;

	USoSAbilityBase* GetAbility() const;

	FDecisionContext& GetDecisionContext();

	TArray<FConsiderationPreset>& GetConsiderations();

	void SetIsAbilityDecision(bool NewValue);

	void SetActionDecisionData(FActionDecisionData* NewDecisionData);

	void SetAbilityDecisionData(FAbilityDecisionData* NewDecisionData);

	void SetAbility(USoSAbilityBase* NewAbility);
};
