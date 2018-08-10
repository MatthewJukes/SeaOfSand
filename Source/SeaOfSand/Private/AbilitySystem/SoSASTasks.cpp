// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "SoSASEffect.h"


bool USoSASTasks::ApplyEffectToTarget(FASEffectData* EffectToApply, AActor* Target, AActor* Instigator)
{
	if (Target == nullptr || Instigator == nullptr)
	{
		return false;
	}

	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		return false;
	}

	TargetASComp->SetASAttribute(EffectToApply->AttributeToEffect, EffectToApply->Value, EffectToApply->Duration);

	return true;
} 
