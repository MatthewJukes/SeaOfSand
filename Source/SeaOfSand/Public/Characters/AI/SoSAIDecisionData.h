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

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI", meta = (ShowOnlyInnerProperties))
	FResponseCurveData ResponseCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI", meta = (ShowOnlyInnerProperties))
	FConsiderationParameter Parameter;
};


USTRUCT(BlueprintType)
struct FConsiderationPreset
{
	GENERATED_USTRUCT_BODY()

		FConsiderationPreset()
		: DataTable(nullptr)
		, RowName(NAME_None)
		, Consideration()
	{
		//ConstructorHelpers::FObjectFinder<UDataTable> DT_DamageTypes(TEXT("DataTable'/Game/CombatSystem/Data/DataTable_DamageTypes.DataTable_DamageTypes'"));
		DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("DataTable'/Game/Characters/AI/UAI_Considerations.UAI_Considerations'")));
	}

	UPROPERTY(VisibleDefaultsOnly, Category = ConsiderationPreset)
	const class UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConsiderationPreset)
	FName RowName;

	UPROPERTY(EditDefaultsOnly, Category = ConsiderationPreset)
	FDecisionConsideration Consideration;
};


USTRUCT(BlueprintType)
struct FDecisionData : public FTableRowBase
{
	GENERATED_BODY()

	FDecisionContext DecisionContext;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	EDecisionAction Action;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI", meta = (ShowOnlyInnerProperties))
	TArray<FDataTableRowHandle> Considerations;
};
