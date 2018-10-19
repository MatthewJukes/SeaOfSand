// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SeaOfSand.h"
#include "SoSCombatComponent.h"
#include "SoSAbilityBase.h"
#include "SoSProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Engine/Classes/Animation/AnimationAsset.h"
#include "Engine/Classes/Curves/CurveFloat.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


bool USoSASTasks::ApplyEffectToTarget(const AActor* Target, AActor* Source, FEffectData& EffectToApply, int32 StackToApply, float EffectDuration)
{ 
	if (Target == nullptr || Source == nullptr)
	{
		return false;
	}
	
	USoSCombatComponent* TargetCombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));
	if (TargetCombatComp == nullptr)
	{
		return false;
	}

	// Check blocked by tags
	int EffectIndex;
	TArray<EAbilityTag>& TargetTags = TargetCombatComp->GetCurrentEffectTags();
	for (EAbilityTag Tag : EffectToApply.EffectBlockedByTags)
	{
		if (TargetTags.Contains(Tag))
		{
			return false;
		}
	}

	// Check required tags
	for (EAbilityTag Tag : EffectToApply.EffectRequiresTags)
	{
		if (!TargetTags.Contains(Tag))
		{
			return false;
		}
	}

	// Set effect source
	EffectToApply.Source = Source;

	// Set duration to infinite for effects with no duration
	EffectToApply.EffectDuration = EffectDuration == 0.0f ? INFINITY : EffectDuration;

	// Check to see if effect already exists on target
	float ApplicationTime = AbilityGetWorldFromContextObject(Source)->GetTimeSeconds();
	TArray<FEffectData>& TargetCurrentEffectsArray = TargetCombatComp->GetCurrentEffectsArray();
	if (CheckIfTargetHasEffectActive(Target, EffectToApply.EffectName, EffectIndex)) // Reapply effect and add stacks if appropriate
	{
		ReapplyEffect(TargetCurrentEffectsArray[EffectIndex], EffectToApply, StackToApply, ApplicationTime);
		TargetCombatComp->OnEffectUpdate.Broadcast(TargetCombatComp, TargetCurrentEffectsArray[EffectIndex], EEffectUpdateEventType::Reapplied);
	}
	else // Apply effect to target
	{
		// Create ability instances
		USoSCombatComponent* SourceCombatComp = Cast<USoSCombatComponent>(Source->GetComponentByClass(USoSCombatComponent::StaticClass()));
		for (FEffectAbilityModule& Module : EffectToApply.AbilityModules)
		{
			Module.Ability = CreateAbilityInstance(Module.AbilityClass, SourceCombatComp);
		}

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
		TargetCombatComp->AddEffectToArray(EffectToApply);
	} 

	return true;
} 


bool USoSASTasks::CheckIfTargetHasEffectActive(const AActor* Target, FName EffectName, int32& OutIndex)
{
	if (Target == nullptr)
	{
		OutIndex = -1;
		return false;
	}

	USoSCombatComponent* TargetCombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));
	if (TargetCombatComp == nullptr)
	{
		OutIndex = -1;
		return false;
	}

	OutIndex = 0;
	TArray<FEffectData>& TargetCurrentEffectsArray = TargetCombatComp->GetCurrentEffectsArray();
	for (FEffectData& Effect : TargetCurrentEffectsArray)
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


bool USoSASTasks::DamageTarget(const AActor* Target, const AActor* Source, float Value, ESoSDamageTypeName DamageType)
{
	if (Target == nullptr || Value <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Target"))
		return false;
	}

	USoSCombatComponent* CombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));

	if (CombatComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target has no CombatComp"))
		return false;
	}

	CombatComp->DamageCalculation(Value, DamageType);
	return true;
}


FVector USoSASTasks::GetAimHitLocation(const AActor* Target)
{
	USoSCombatComponent* CombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));

	if (CombatComp == nullptr)
	{
		return FVector::ZeroVector;
	}

	return *CombatComp->GetAimHitLocation();
}


bool USoSASTasks::WeaponTrace(const AActor* Source, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation)
{
	if (Source == nullptr)
	{
		return false;
	}

	UWorld* World = AbilityGetWorldFromContextObject(Source);
	if (World == nullptr)
	{
		return false;
	}

	const FName TraceTag("WeaponTraceTag");
	//World->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Source);
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


bool USoSASTasks::FireProjectile(AActor* Source, TSubclassOf<ASoSProjectileBase> Projectile, const FTransform &SpawnTransform)
{
	if (Projectile == nullptr || Source == nullptr)
	{
		return false;
	}

	UWorld* World = AbilityGetWorldFromContextObject(Source);
	if (World == nullptr)
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Source;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASoSProjectileBase* NewProjectile = World->SpawnActor<ASoSProjectileBase>(Projectile, SpawnTransform, SpawnParams);

	return NewProjectile != nullptr;
}


bool USoSASTasks::FireProjectileFromWeaponAtAimLocation(AActor* Source, TSubclassOf<ASoSProjectileBase> Projectile, const FVector &SocketLocation)
{
	FHitResult Hit;
	FVector EndLocation = GetAimHitLocation(Source);
	if (WeaponTrace(Source, Hit, SocketLocation, EndLocation))
	{
		EndLocation = Hit.Location;
	}
	
	// Convert end location to direction
	EndLocation = EndLocation - SocketLocation;
	EndLocation.Normalize();

	FTransform ProjectileTransform = FTransform(FRotator(EndLocation.ToOrientationRotator()), SocketLocation);
	if (!FireProjectile(Source, Projectile, ProjectileTransform))
	{
		return false;
	}

	return true;
}


bool USoSASTasks::MeleeHitCheck(const AActor* Source, AActor* Target, TArray<AActor*>& PreviouslyHitActors)
{
	if (Source == nullptr || Target == nullptr)
	{
		return false;
	}

	if (Source == Target)
	{
		return false;
	}

	for (AActor* Actor : PreviouslyHitActors)
	{
		if (Target == Actor)
		{
			return false;
		}
	}

	// Add target to array so it can't be hit again
	PreviouslyHitActors.Add(Target);
	return true;
}


bool USoSASTasks::GetTargetsInRadius(const AActor* Source, TArray<FHitResult> &OutHitResults, const FVector &Origin, float Radius)
{
	TArray<AActor*> Targets;

	if (Source == nullptr)
	{
		return false;
	}

	UWorld* World = AbilityGetWorldFromContextObject(Source);
	if (World == nullptr)
	{
		return false;
	}

	DrawDebugSphere(World, Origin, Radius, 100, FColor::Purple, true, 0.25f);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Source);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	return World->SweepMultiByChannel(OutHitResults, Origin, Origin, FQuat::Identity, COLLISION_ABILITY, Shape, TraceParams);
}


ESoSTeam USoSASTasks::GetTeamFromTarget(const AActor* Target)
{
	if (Target == nullptr)
	{
		return ESoSTeam::Default;
	}

	USoSCombatComponent* TargetCombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));

	if (TargetCombatComp == nullptr)
	{
		return ESoSTeam::Default;
	}

	return TargetCombatComp->GetTeam();
}


bool USoSASTasks::TeamCheck(const AActor* ActorOne, const AActor* ActorTwo)
{
	if (ActorOne == ActorTwo)
	{
		return true;
	}

	if (ActorOne == nullptr || ActorTwo == nullptr)
	{
		return false;
	}

	USoSCombatComponent* CombatCompOne = Cast<USoSCombatComponent>(ActorOne->GetComponentByClass(USoSCombatComponent::StaticClass()));
	USoSCombatComponent* CombatCompTwo = Cast<USoSCombatComponent>(ActorTwo->GetComponentByClass(USoSCombatComponent::StaticClass()));
	if (CombatCompOne == nullptr || CombatCompTwo == nullptr)
	{
		return false;
	}

	if (CombatCompOne->GetTeam() == CombatCompTwo->GetTeam())
	{
		return true;
	}

	return false;
}


bool USoSASTasks::ApplyRootMotionConstantForce(const ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish)
{
	if (TargetCharacter == nullptr)
	{
		return false;
	}

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(TargetCharacter->GetCharacterMovement());
	if (MovementComp == nullptr)
	{
		return false;
	}

	Direction.Normalize();

	FRootMotionSource_ConstantForce* ConstantForce = new FRootMotionSource_ConstantForce();
	ConstantForce->InstanceName = FName("ConstantForce");
	ConstantForce->AccumulateMode = bIsAdditive ? ERootMotionAccumulateMode::Additive : ERootMotionAccumulateMode::Override;
	ConstantForce->Priority = 5;
	ConstantForce->Force = Direction * Strength;
	ConstantForce->Duration = Duration;
	ConstantForce->StrengthOverTime = StrengthOverTime;
	ConstantForce->FinishVelocityParams.Mode = VelocityOnFinishMode;
	ConstantForce->FinishVelocityParams.SetVelocity = SetVelocityOnFinish;
	ConstantForce->FinishVelocityParams.ClampVelocity = ClampVelocityOnFinish;
	MovementComp->ApplyRootMotionSource(ConstantForce);

	return true;
}


bool USoSASTasks::ApplyRootMotionJumpForce(const ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve)
{
	if (TargetCharacter == nullptr)
	{
		return false;
	}

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(TargetCharacter->GetCharacterMovement());
	if (MovementComp == nullptr)
	{
		return false;
	}

	FRootMotionSource_JumpForce* JumpForce = new FRootMotionSource_JumpForce();
	JumpForce->InstanceName = FName("JumpForce");
	JumpForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	JumpForce->Priority = 500;
	JumpForce->Duration = Duration;
	JumpForce->Rotation = Rotation;
	JumpForce->Distance = Distance;
	JumpForce->Height = Height;
	JumpForce->Duration = Duration;
	JumpForce->bDisableTimeout = bFinishOnLanded; // If we finish on landed, we need to disable force's timeout
	JumpForce->PathOffsetCurve = PathOffsetCurve;
	JumpForce->TimeMappingCurve = TimeMappingCurve;
	JumpForce->FinishVelocityParams.Mode = VelocityOnFinishMode;
	JumpForce->FinishVelocityParams.SetVelocity = SetVelocityOnFinish;
	JumpForce->FinishVelocityParams.ClampVelocity = ClampVelocityOnFinish;
	MovementComp->ApplyRootMotionSource(JumpForce);

	return true;
}


bool USoSASTasks::PlayAbilityAnimMontage(USoSAbilityBase* SourceAbility, ACharacter* Target, UAnimMontage* AnimMontage, float PlayRate, FName StartSectionName)
{
	if (SourceAbility == nullptr || Target == nullptr || AnimMontage == nullptr)
	{
		return false;
	}

	USoSCombatComponent* TargetCombatComp = Cast<USoSCombatComponent>(Target->GetComponentByClass(USoSCombatComponent::StaticClass()));
	if (TargetCombatComp == nullptr)
	{
		return false;
	}

	TargetCombatComp->SetLastAbilityToStartMontage(SourceAbility);

	Target->PlayAnimMontage(AnimMontage, PlayRate, StartSectionName);

	return true;
}


USoSAbilityBase * USoSASTasks::CreateAbilityInstance(TSubclassOf<USoSAbilityBase> Ability, USoSCombatComponent* OwningCombatComp)
{
	if (Ability == nullptr)
	{
		return nullptr;
	}

	USoSAbilityBase* AbilityInstance = NewObject<USoSAbilityBase>(Ability, Ability.Get());
	AbilityInstance->SetOwningCombatComp(OwningCombatComp);
	AbilityInstance->InitializeAbility();
	return AbilityInstance;
}


void USoSASTasks::ReapplyEffect(FEffectData& ExistingEffect, FEffectData& NewEffect, int32 StackToApply, float ApplicationTime)
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


UWorld* USoSASTasks::AbilityGetWorldFromContextObject(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr || GEngine == nullptr)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return nullptr;
	}

	return World;
}
