// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSEffectData.h"
#include "SoSASAttributes.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

class ASoSProjectileBase;
class USoSAbilityBase;
class USoSCombatComponent;
class ACharacter;
class UWorld;
class UAnimMontage;
enum class ERootMotionFinishVelocityMode : uint8;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ApplyEffectToTarget(const AActor* Target, AActor* Source, UPARAM(ref) FEffectData& EffectToApply, int32 StackToApply, float EffectDuration);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool CheckIfTargetHasEffectActive(const AActor* Target, FName EffectName, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool DamageTarget(const AActor* Target, const AActor* Source, float Value, ESoSDamageTypeName DamageType);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool AddValueToTargetsAttribute(const AActor* Target, const AActor* Source, EAttributeName Attribute, float Value);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static FVector GetAimHitLocation(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool WeaponTrace(const AActor* Source, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool FireProjectile(AActor* Source, TSubclassOf<ASoSProjectileBase> Projectile, const FTransform &SpawnTransform, float ProjectileDamage, float ProjectileSpeed);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool FireProjectileFromWeaponAtAimLocation(AActor* Source, TSubclassOf<ASoSProjectileBase> Projectile, const FVector &SocketLocation, float ProjectileDamage, float ProjectileSpeed, float ProjectileSpread = 0);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool MeleeHitCheck(AActor* Target, const AActor* Source, UPARAM(ref) TArray<AActor*>& PreviouslyHitActors);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool GetTargetsInRadius(const AActor* Source, TArray<FHitResult> &OutHitResults, const FVector &Origin, float Radius);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static ESoSTeam GetTeamFromTarget(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool TeamCheck(const AActor* ActorOne, const AActor* ActorTwo);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ApplyRootMotionConstantForce(const ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ApplyRootMotionJumpForce(const ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve);
	
	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool PlayAbilityAnimMontage(USoSAbilityBase* SourceAbility, ACharacter* Target, UAnimMontage* AnimMontage, float PlayRate = 1.0f, FName StartSectionName = "None");

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static float GetTimeSeconds(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool CalculateEclipsePoints(AActor* Source, float EclipseRatio, int32 &SunPoints, int32 &MoonPoints);

	static USoSAbilityBase* CreateAbilityInstance(TSubclassOf<USoSAbilityBase> Ability, USoSCombatComponent* OwningCombatComp);

private:

	static void ReapplyEffect(FEffectData& ExistingEffect, FEffectData& NewEffect, int32 StackToApply, float ApplicationTime);

	UFUNCTION(meta = (WorldContext = "Instigator"), Category = "AbilityTask")
	static UWorld* AbilityGetWorldFromContextObject(const UObject* WorldContextObject);
};
