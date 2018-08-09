// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASTasks.generated.h"

class USoSASBuff;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UObject
{
	GENERATED_BODY()

protected:

	bool ApplyBuff(USoSASBuff* BuffToApply, AActor* Target, AActor* Instigator);	
	
};
