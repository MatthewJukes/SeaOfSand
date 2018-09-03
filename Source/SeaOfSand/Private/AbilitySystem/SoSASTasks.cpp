// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "Engine/World.h"


bool USoSASTasks::ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, float EffectDuration, float ApplicationTime)
{ 
	//UE_LOG(LogTemp, Warning, TEXT("Effect Name: %s Effect Duration: %f"), *EffectToApply.EffectName.ToString(), EffectToApply.EffectDuration);

	if (Target == nullptr || Instigator == nullptr)
	{
		return false;
	}

	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		return false;
	}

	// Set duration to infinite for effects with no duration
	EffectToApply.EffectDuration = EffectDuration == 0.0f ? INFINITY : EffectDuration;

	// Check to see if effect already exists on target
	TArray<FASEffectData>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray(EffectToApply.EffectType);
	int EffectIndex;
	if (CheckIfTargetHasASEffectActive(EffectToApply, Target, EffectIndex)) // Reapply effect and add stacks if appropriate TODO remake existing check
	{
		ReapplyASEffect(TargetCurrentEffectsArray[EffectIndex], EffectToApply, ApplicationTime);
	}
	else // Apply effect to target
	{
		// Set effect status trackers
		EffectToApply.EffectStartTime = ApplicationTime;
		EffectToApply.CurrentStacks = FMath::Clamp(EffectToApply.StacksPerApplication, 1, EffectToApply.MaxStacks);

		// Set tick rate to effect duration for effects with a tick rate of zero
		if (EffectToApply.TickRate == 0.0f)
		{
			EffectToApply.TickRate = EffectToApply.EffectDuration;
		}

		// Set last tick time
		EffectToApply.LastTickTime = EffectToApply.bDelayFirstTick ? ApplicationTime : ApplicationTime - EffectToApply.TickRate;

		// Add effect to array
		TargetASComp->AddASEffectToArray(EffectToApply);
	} 

	return true;
} 

bool USoSASTasks::CheckIfTargetHasASEffectActive(FASEffectData& EffectToCheck, AActor* Target, int32& OutIndex)
{
	if (Target == nullptr)
	{
		OutIndex = -1;
		return false;
	}

	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		OutIndex = -1;
		return false;
	}

	OutIndex = 0;
	TArray<FASEffectData>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray(EffectToCheck.EffectType);
	for (FASEffectData& Effect : TargetCurrentEffectsArray)
	{
		if (Effect.EffectName == EffectToCheck.EffectName)
		{
			return true;
		}
		OutIndex++;
	}

	OutIndex = -1;
	return false;
}

void USoSASTasks::ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, float ApplicationTime)
{
	if (NewEffect.bAdditiveDuration && ExistingEffect.bAdditiveDuration)
	{
		ExistingEffect.EffectDuration += NewEffect.EffectDuration;
	}
	else if(NewEffect.EffectDuration > ApplicationTime - ExistingEffect.EffectStartTime)
	{
		ExistingEffect.EffectDuration = NewEffect.EffectDuration;
		ExistingEffect.EffectStartTime = ApplicationTime;
	}

	ExistingEffect.CurrentStacks = FMath::Clamp(ExistingEffect.CurrentStacks + NewEffect.StacksPerApplication, 1, NewEffect.MaxStacks);
}
