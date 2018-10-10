// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASEffectData.h"

#if WITH_EDITOR
bool FASEffectData::CanEditChange(const UProperty* InProperty) const
{
	//const bool ParentVal = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FASEffectData, AttributeToEffect))
	{
		return EffectType == EASEffectType::AttributeModifier;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FASEffectData, DamageType))
	{
		return EffectType == EASEffectType::DamageDealer;
	}

	return false;
}
#endif
