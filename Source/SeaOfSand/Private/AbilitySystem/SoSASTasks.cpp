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

	switch (EffectToApply->EffectValueType)
	{
	case EASEffectValueType::Additive:
		AddValueToASAttribute(TargetASComp, EffectToApply->AttributeToEffect, EffectToApply->EffectValue);
		break;
	case EASEffectValueType::Multiplicative:
		break;
	case EASEffectValueType::Subtractive:
		break;
	default:
		break;
	}

	return true;
} 

void USoSASTasks::AddValueToASAttribute(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value)
{
	USoSASComponent* TargetASComp = Cast<USoSASComponent>(ASComponent);

	if (TargetASComp == nullptr)
	{
		return;
	}

	TargetASComp->SetASAttribute(AttributeToEffect, Value);
}

void USoSASTasks::AddMultiplierToASAttribute(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value)
{

}

void USoSASTasks::AddDividerToASAttribte(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value)
{

}
