/************************************************************************************************************************************
*
*	SoSAICharacterBase.cpp
*	AI character base 
*
*	Written by Matthew Jukes (20/11/2018)
*	Owned by Matthew Jukes
*
****/


#include "SoSAICharacterBase.h"


ASoSAICharacterBase::ASoSAICharacterBase()
{

}


void ASoSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


TArray<FDecision>& ASoSAICharacterBase::GetDecisions()
{
	return Decisions;
}
