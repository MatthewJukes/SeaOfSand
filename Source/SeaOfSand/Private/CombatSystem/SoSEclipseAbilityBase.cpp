// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSEclipseAbilityBase.h"
#include "SoSASTasks.h"




USoSEclipseAbilityBase::USoSEclipseAbilityBase()
{
	SunPoints = 3;
	MoonPoints = 3;
}

bool USoSEclipseAbilityBase::StartAbility_Implementation(AActor* Source, ASoSWeaponBase* Weapon, float ClassSpecificFloatValue)
{
	return USoSASTasks::CalculateEclipsePoints(Source, ClassSpecificFloatValue, SunPoints, MoonPoints);	
} 


bool USoSEclipseAbilityBase::ReleashAbility_Implementation(AActor* Source, ASoSWeaponBase* Weapon, float ClassSpecificFloatValue)
{
	return USoSASTasks::CalculateEclipsePoints(Source, ClassSpecificFloatValue, SunPoints, MoonPoints);
}  


int32 USoSEclipseAbilityBase::GetSunPoints() const
{
	return SunPoints;
}


int32 USoSEclipseAbilityBase::GetMoonPoints() const
{
	return MoonPoints;
}
