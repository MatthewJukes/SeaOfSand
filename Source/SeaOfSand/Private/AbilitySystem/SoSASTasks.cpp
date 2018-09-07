// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "Engine/World.h"


bool USoSASTasks::ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, float EffectDuration, float ApplicationTime)
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

	// Check blocked by tags
	int EffectIndex;
	TArray<EASTag>& TargetTags = TargetASComp->GetCurrentTags();
	for (EASTag Tag : EffectToApply.EffectBlockedByTags)
	{
		if (TargetTags.Contains(Tag))
		{
			return false;
		}
	}

	// Check required tags
	for (EASTag Tag : EffectToApply.EffectRequiresTags)
	{
		if (!TargetTags.Contains(Tag))
		{
			return false;
		}
	}

	// Set duration to infinite for effects with no duration
	EffectToApply.EffectDuration = EffectDuration == 0.0f ? INFINITY : EffectDuration;

	// Check to see if effect already exists on target
	TArray<FASEffectData>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray();
	if (CheckIfTargetHasASEffectActive(EffectToApply.EffectName, Target, EffectIndex)) // Reapply effect and add stacks if appropriate
	{
		ReapplyASEffect(TargetCurrentEffectsArray[EffectIndex], EffectToApply, ApplicationTime);
	}
	else // Apply effect to target
	{
		// Set effect status trackers
		EffectToApply.EffectStartTime = ApplicationTime;
		EffectToApply.NewStacks = FMath::Clamp(EffectToApply.StacksPerApplication, 0, EffectToApply.MaxStacks);
		EffectToApply.CurrentStacks = FMath::Clamp(EffectToApply.StacksPerApplication, 1, EffectToApply.MaxStacks);

		// Set tick rate to effect duration for effects with a tick rate of zero
		EffectToApply.bNonTicking = EffectToApply.TickRate == 0.0f;
		EffectToApply.TickRate = EffectToApply.TickRate == 0.0f ? EffectToApply.EffectDuration : EffectToApply.TickRate;

		// Set last tick time
		EffectToApply.LastTickTime = EffectToApply.bDelayFirstTick ? ApplicationTime : ApplicationTime - EffectToApply.TickRate;

		// Add effect to array
		TargetASComp->AddASEffectToArray(EffectToApply);
	} 

	return true;
} 

bool USoSASTasks::CheckIfTargetHasASEffectActive(FName EffectName, AActor* Target, int32& OutIndex)
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
	TArray<FASEffectData>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray();
	for (FASEffectData& Effect : TargetCurrentEffectsArray)
	{
		if (Effect.EffectName == EffectName)
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

	if (ExistingEffect.bNonTicking)
	{
		ExistingEffect.TickRate = ExistingEffect.EffectDuration;
		ExistingEffect.LastTickTime = ExistingEffect.bDelayFirstTick ? ApplicationTime : ApplicationTime - ExistingEffect.TickRate;
	}

	ExistingEffect.NewStacks = FMath::Clamp(NewEffect.StacksPerApplication, 0, ExistingEffect.MaxStacks - ExistingEffect.CurrentStacks);
	ExistingEffect.CurrentStacks = FMath::Clamp(ExistingEffect.CurrentStacks + NewEffect.StacksPerApplication, 1, ExistingEffect.MaxStacks);
}
