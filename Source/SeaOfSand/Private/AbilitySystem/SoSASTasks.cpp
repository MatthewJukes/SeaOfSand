// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASTasks.h"
#include "SeaOfSand.h"
#include "SoSASComponent.h"
#include "SoSASAbilityBase.h"
#include "SoSASProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Engine/Classes/Animation/AnimationAsset.h"
#include "Engine/Classes/Curves/CurveFloat.h"
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
		TargetASComp->OnEffectUpdate.Broadcast(TargetASComp, TargetCurrentEffectsArray[EffectIndex], EASEffectUpdateEventType::Reapplied);
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


bool USoSASTasks::ASDamageTarget(AActor* Target, float Value)
{
	if (Target == nullptr || Value <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Target"))
		return false;
	}

	USoSASComponent* ASComp = Cast<USoSASComponent>(Target->GetComponentByClass(USoSASComponent::StaticClass()));

	if (ASComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target has no ASComp"))
		return false;
	}

	ASComp->AddValueToASAttributeBaseValues(EASAttributeName::HealthCurrent, -Value);
	return true;
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
	//World->DebugDrawTraceTag = TraceTag;

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


bool USoSASTasks::ASPlayAnimation(USkeletalMeshComponent* Mesh, UAnimationAsset* Animation, bool bIsLooping)
{
	if (Mesh == nullptr || Animation == nullptr)
	{
		return false;
	}

	Mesh->PlayAnimation(Animation, bIsLooping);
	return true;
}

bool USoSASTasks::ASApplyRootMotionConstantForce(ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish)
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


bool USoSASTasks::ASApplyRootMotionJumpForce(ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve)
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

USoSASAbilityBase * USoSASTasks::CreateASAbilityInstance(TSubclassOf<USoSASAbilityBase> Ability)
{
	if (Ability == nullptr)
	{
		return nullptr;
	}

	return NewObject<USoSASAbilityBase>(Ability, Ability.Get());
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
