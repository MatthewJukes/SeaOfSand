/************************************************************************************************************************************
*
*	SoSAIDecisionData.h
*	Utility-theory AI decision data structs
*
*	Written by Matthew Jukes (29/11/2018)
*	Owned by Matthew Jukes
*
****/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoSAIDecisionData.generated.h"

class ASoSCharacterBase;
class USoSAbilityBase;
class UCurveFloat;


/** Enum of possible consideration parameters, mapped to a normalizing function */
UENUM(BlueprintType)
enum class EConsiderationParameters : uint8
{
	MyHealth,
	MyDistanceToTarget,
	MyDirectionOffsetToTarget,
	MyLineOfSightToTarget,
	TargetHealth,
	TargetDirectionOffsetToMe
};


/** Enum of possible action to take, mapped to an action function */
UENUM(BlueprintType)
enum class EDecisionAction : uint8
{
	NoAction,
	MoveToActor,
	MoveToLocation
};


/** Context data the decision is being made in, such as what target we are scoring for */
USTRUCT(BlueprintType)
struct FDecisionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	ASoSCharacterBase* SourceCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	AActor* Target;
};


/** What parameters to use for getting the normalized value */
USTRUCT(BlueprintType)
struct FConsiderationParameter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	EConsiderationParameters ConsiderationParameter;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float RangeMin;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float RangeMax;
};


/** Curve and additional parameters to use when evaluating the normalized value and returning a score */
USTRUCT(BlueprintType)
struct FResponseCurveData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	UCurveFloat* Curve;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float OffsetX;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float OffsetY;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetSlope;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float Exponent;
};


/** The Consideration  */
USTRUCT(BlueprintType)
struct FDecisionConsideration : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI", meta = (ShowOnlyInnerProperties))
	FResponseCurveData ResponseCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI", meta = (ShowOnlyInnerProperties))
	FConsiderationParameter Parameter;
};


/** Struct for selecting consideration presets from a data table */
USTRUCT(BlueprintType)
struct FConsiderationPreset
{
	GENERATED_BODY()

		FConsiderationPreset()
		: DataTable(nullptr)
		, RowName(NAME_None)
		, Consideration()
	{
		DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("DataTable'/Game/Characters/AI/UAI_Considerations.UAI_Considerations'")));
	}

	UPROPERTY(VisibleDefaultsOnly, Category = "Utility AI")
	const class UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
	FName RowName;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FDecisionConsideration Consideration;
};


/** Struct for selecting action decision data from a data table for AI characters */
USTRUCT(BlueprintType)
struct FAIActionDecision
{
	GENERATED_BODY()

		FAIActionDecision()
		: DataTable(nullptr)
		, RowName(NAME_None)
	{
		DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("DataTable'/Game/Characters/AI/UAI_ActionDecisions.UAI_ActionDecisions'")));
	}

	UPROPERTY(VisibleDefaultsOnly, Category = "Utility AI")
	const class UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
	FName RowName;
};


/** Struct for selecting decision data from a data table for AI characters */
USTRUCT(BlueprintType)
struct FAIAbilityDecision
{
	GENERATED_BODY()

		FAIAbilityDecision()
		: DataTable(nullptr)
		, RowName(NAME_None)
	{
		DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("DataTable'/Game/Characters/AI/UAI_AbilityDecisions.UAI_AbilityDecisions'")));
	}

	UPROPERTY(VisibleDefaultsOnly, Category = "Utility AI")
	const class UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
	FName RowName;
};


/** Decision data for non-skill action, mapped to a single action */
USTRUCT(BlueprintType)
struct FActionDecisionData : public FTableRowBase
{
	GENERATED_BODY()

	FDecisionContext DecisionContext;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	EDecisionAction Action;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TArray<FConsiderationPreset> Considerations;
};


/** Decision data for skill. not assigned to any particular skill */
USTRUCT(BlueprintType)
struct FAbilityDecisionData : public FTableRowBase
{
	GENERATED_BODY()

	FDecisionContext DecisionContext;

	UPROPERTY()
	USoSAbilityBase* Ability;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TArray<FConsiderationPreset> Considerations;
};