// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASAbilityBase.h"
#include "Engine/World.h"


float USoSASAbilityBase::GetGameTimeSeconds() const
{
	return GetWorld()->GetTimeSeconds();
}
