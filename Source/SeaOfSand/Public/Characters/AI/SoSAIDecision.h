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
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "SoSAIDecision.generated.h"


class ASoSCharacterBase;
class UCurveFloat;


UENUM(BlueprintType)
enum class EConsiderationParameters : uint8
{
	MyHealth,
	MyDistanceToTarget,
	MyDirectionOffsetToTarget,
	MyLineOfSightToTarget,
	TargetHealth
};


UENUM(BlueprintType)
enum class EDecisionAction : uint8
{
	MoveToActor,
	MoveToLocation
};


USTRUCT(BlueprintType)
struct FDecisionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	ASoSCharacterBase* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	AActor* Target;
};


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


USTRUCT(BlueprintType)
struct FResponseCurveData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	UCurveFloat* Curve;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetX;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetY;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetSlope;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float Exponent;
};


USTRUCT(BlueprintType)
struct FDecisionConsideration : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FResponseCurveData ResponseCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FConsiderationParameter Parameter;
};


USTRUCT(BlueprintType)
struct FConsiderationPreset : public FDataTableRowHandle
{
	GENERATED_BODY()

};


USTRUCT(BlueprintType)
struct FDecisionData : public FTableRowBase
{
	GENERATED_BODY()

	FDecisionContext DecisionContext;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	EDecisionAction Action;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TArray<FConsiderationPreset> Considerations;
};


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

public:

	//EDecisionAction GetAction() const;

	//FDecisionContext& GetDecisionContext();

	//TArray<FDecisionConsideration>& GetConsiderations();
};
