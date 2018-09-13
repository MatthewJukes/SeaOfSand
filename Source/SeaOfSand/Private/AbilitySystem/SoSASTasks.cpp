// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SeaOfSand.h"
#include "SoSASComponent.h"
#include "SoSASProjectileBase.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Engine/World.h"


bool USoSASTasks::ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, int32 StackToApply, float EffectDuration, float ApplicationTime)
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
	TArray<EASTag>& TargetTags = TargetASComp->GetCurrentASEffectTags();
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
		ReapplyASEffect(TargetCurrentEffectsArray[EffectIndex], EffectToApply, StackToApply, ApplicationTime);
	}
	else // Apply effect to target
	{
		// Set effect status trackers
		EffectToApply.EffectStartTime = ApplicationTime;
		EffectToApply.NewStacks = FMath::Clamp(StackToApply, 0, EffectToApply.MaxStacks);
		EffectToApply.CurrentStacks = FMath::Clamp(StackToApply, 1, EffectToApply.MaxStacks);

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


FVector USoSASTasks::ASGetAimHitLocation(AActor* Actor)
{
	USoSASComponent* ASComp = Cast<USoSASComponent>(Actor->GetComponentByClass(USoSASComponent::StaticClass()));

	if (ASComp == nullptr)
	{
		return FVector::ZeroVector;
	}

	return *ASComp->GetAimHitLocation();
}


bool USoSASTasks::ASWeaponTrace(AActor* Instigator, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, UWorld* World)
{
	const FName TraceTag("WeaponTraceTag");
	World->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Instigator);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.TraceTag = TraceTag;

	if (World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, COLLISION_WEAPON, TraceParams))
	{
		return true;
	}
	return false; // Line-trace didn't hit anything
}


bool USoSASTasks::FireASProjectile(TSubclassOf<ASoSASProjectileBase> Projectile, const FTransform &SpawnTransform, AActor* Instigator,  UWorld* World)
{
	if (Projectile == nullptr || Instigator == nullptr || World == nullptr)
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASoSASProjectileBase* NewProjectile = World->SpawnActor<ASoSASProjectileBase>(Projectile, SpawnTransform, SpawnParams);

	return NewProjectile != nullptr;
}


bool USoSASTasks::FireASProjectileFromWeaponAtAimLocation(TSubclassOf<ASoSASProjectileBase> Projectile, AActor* Instigator, const FVector &SocketLocation, UWorld* World)
{
	FHitResult Hit;
	FVector EndLocation = ASGetAimHitLocation(Instigator);
	if (ASWeaponTrace(Instigator, Hit, SocketLocation, EndLocation, World))
	{
		EndLocation = Hit.Location;
	}
	
	// Convert end location to direction
	EndLocation = EndLocation - SocketLocation;
	EndLocation.Normalize();

	FTransform ProjectileTransform = FTransform(FRotator(EndLocation.ToOrientationRotator()), SocketLocation);
	if (!FireASProjectile(Projectile, ProjectileTransform, Instigator, World))
	{
		return false;
	}

	return true;
}

EASTeam USoSASTasks::GetASTeam(const AActor* Target)
{
	USoSASComponent* TargetASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (TargetASComp == nullptr)
	{
		return EASTeam::Default;
	}

	return TargetASComp->GetASTeam();
}


void USoSASTasks::ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, int32 StackToApply, float ApplicationTime)
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

	ExistingEffect.NewStacks = FMath::Clamp(StackToApply, 0, ExistingEffect.MaxStacks - ExistingEffect.CurrentStacks);
	ExistingEffect.CurrentStacks = FMath::Clamp(ExistingEffect.CurrentStacks + StackToApply, 1, ExistingEffect.MaxStacks);
}
