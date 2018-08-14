// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "SoSASEffect.h"
#include "Engine/World.h"


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

	// Check to see if effect already exists on target
	TArray<FASEffectData*>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray(EffectToApply->EffectType);
	int EffectIndex;
	if (TargetCurrentEffectsArray.Find(EffectToApply, EffectIndex)) // Reapply effect and add stacks if appropriate
	{
		// Update effect status checkers
		TargetCurrentEffectsArray[EffectIndex]->EffectStartTime = GetWorld()->GetTimeSeconds();
		TargetCurrentEffectsArray[EffectIndex]->CurrentStacks = FMath::Clamp(TargetCurrentEffectsArray[EffectIndex]->CurrentStacks + EffectToApply->StacksPerApplication, 1, EffectToApply->MaxStacks);
	}
	else // Apply effect to target
	{
		// Set effect status trackers
		EffectToApply->EffectStartTime = GetWorld()->GetTimeSeconds();	
		EffectToApply->CurrentStacks = FMath::Clamp(EffectToApply->StacksPerApplication, 1, EffectToApply->MaxStacks);

		// Set duration to infinite for effects with no duration
		if (EffectToApply->EffectDuration == 0.0f)
		{
			EffectToApply->EffectDuration = INFINITY;
		}

		// Set tick rate to effect duration for effects with a tick rate of zero
		if (EffectToApply->TickRate == 0.0f)
		{
			EffectToApply->TickRate = EffectToApply->EffectDuration;
		}

		// Set last tick time
		EffectToApply->TimeSinceLastTick = GetWorld()->GetTimeSeconds() - EffectToApply->TickRate;

		// Delay first tick
		if (EffectToApply->bDelayFirstTick)
		{
			EffectToApply->TimeSinceLastTick = GetWorld()->GetTimeSeconds();
		}

		// Add effect to array
		TargetASComp->AddASEffectToArray(EffectToApply);
	}

	return true;
} 