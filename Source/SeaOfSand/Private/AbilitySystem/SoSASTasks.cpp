// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "SoSASBuffBase.h"


bool USoSASTasks::ApplyBuff(USoSASBuff* BuffToApply, AActor* Target, AActor* Instigator)
{
	if (BuffToApply == nullptr || Target == nullptr || Instigator == nullptr)
	{
		return false;
	}

	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		return false;
	}

	//USoSASBuffBase* Buff = Cast<USoSASBuffBase>(BuffToApply);

	//Buff->ApplyBuff(TargetASComp);

	return true;
} 
