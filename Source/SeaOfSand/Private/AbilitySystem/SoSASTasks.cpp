// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SoSASComponent.h"
#include "Engine/World.h"


bool USoSASTasks::ApplyEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, float ApplicationTime)
{ 
	FASEffectData* NewEffect = &EffectToApply;
	UE_LOG(LogTemp, Warning, TEXT("Effect Name: %s Effect Duration: %f"), *NewEffect->EffectName.ToString(), NewEffect->EffectDuration);

	if (NewEffect == nullptr || Target == nullptr || Instigator == nullptr)
	{
		return false;
	}

	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		return false;
	}

	// Check to see if effect already exists on target
	TArray<FASEffectData*>& TargetCurrentEffectsArray = TargetASComp->GetCurrentEffectsArray(NewEffect->EffectType);
	int EffectIndex;
	if (TargetCurrentEffectsArray.Find(NewEffect, EffectIndex)) // Reapply effect and add stacks if appropriate
	{
		// Update effect status checkers
		TargetCurrentEffectsArray[EffectIndex]->EffectStartTime = ApplicationTime;
		TargetCurrentEffectsArray[EffectIndex]->CurrentStacks = FMath::Clamp(TargetCurrentEffectsArray[EffectIndex]->CurrentStacks + NewEffect->StacksPerApplication, 1, NewEffect->MaxStacks);
	}
	else // Apply effect to target
	{
		// Set effect status trackers
		NewEffect->EffectStartTime = ApplicationTime;
		NewEffect->CurrentStacks = FMath::Clamp(NewEffect->StacksPerApplication, 1, NewEffect->MaxStacks);

		// Set duration to infinite for effects with no duration
		if (NewEffect->EffectDuration == 0.0f)
		{
			NewEffect->EffectDuration = INFINITY;
		}

		// Set tick rate to effect duration for effects with a tick rate of zero
		if (NewEffect->TickRate == 0.0f)
		{
			NewEffect->TickRate = NewEffect->EffectDuration;
		}

		// Set last tick time
		NewEffect->TimeSinceLastTick = NewEffect->bDelayFirstTick ? ApplicationTime - NewEffect->TickRate : ApplicationTime;

		// Add effect to array
		TargetASComp->AddASEffectToArray(NewEffect);
	} 

	Test(NewEffect);
	return true;
} 

void USoSASTasks::Test(FASEffectData* Test)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect Name: %s Effect Duration: %f"), *Test->EffectName.ToString(), Test->EffectDuration);
}
