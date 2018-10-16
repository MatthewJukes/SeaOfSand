// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASEffectData.h"
#include "SoSASAttributes.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

class ASoSASProjectileBase;
class USoSASAbilityBase;
class USoSASComponent;
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
	static bool ApplyEffectToTarget(const AActor* Target, AActor* Source, UPARAM(ref) FASEffectData& EffectToApply, int32 StackToApply, float EffectDuration);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool CheckIfTargetHasEffectActive(const AActor* Target, FName EffectName, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool DamageTarget(const AActor* Target, const AActor* Source, float Value, EASDamageTypeName DamageType);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static FVector ASGetAimHitLocation(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASWeaponTrace(const AActor* Source, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool FireASProjectile(AActor* Source, TSubclassOf<ASoSASProjectileBase> Projectile, const FTransform &SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool FireASProjectileFromWeaponAtAimLocation(AActor* Source, TSubclassOf<ASoSASProjectileBase> Projectile, const FVector &SocketLocation);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASMeleeHitCheck(const AActor* Source, AActor* Target, UPARAM(ref) TArray<AActor*>& PreviouslyHitActors);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASGetTargetsInSphere(const AActor* Source, TArray<FHitResult> &OutHitResults, const FVector &Origin, float Radius);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static ESoSTeam GetTeamFromTarget(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool TeamCheck(const AActor* ActorOne, const AActor* ActorTwo);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASApplyRootMotionConstantForce(const ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish);

	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASApplyRootMotionJumpForce(const ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve);
	
	UFUNCTION(BlueprintCallable, Category = "AbilityTask")
	static bool ASPlayAnimMontage(USoSASAbilityBase* SourceAbility, ACharacter* Target, UAnimMontage* AnimMontage, float PlayRate = 1.0f, FName StartSectionName = "None");

	static USoSASAbilityBase* CreateASAbilityInstance(TSubclassOf<USoSASAbilityBase> Ability, USoSASComponent* OwningASComp);

private:

	static void ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, int32 StackToApply, float ApplicationTime);

	UFUNCTION(meta = (WorldContext = "Instigator"), Category = "AbilityTask")
	static UWorld* ASGetWorldFromContextObject(const UObject* WorldContextObject);
};
