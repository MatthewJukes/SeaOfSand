// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASEffectData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

class ASoSASProjectileBase;
class USoSASAbilityBase;
class ACharacter;
class UWorld;
enum class ERootMotionFinishVelocityMode : uint8;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, int32 StackToApply, float EffectDuration, float ApplicationTime);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool CheckIfTargetHasASEffectActive(FName EffectName, AActor* Target, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static FVector ASGetAimHitLocation(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASWeaponTrace(AActor* Instigator, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool FireASProjectile(TSubclassOf<ASoSASProjectileBase> Projectile, const FTransform &SpawnTransform, AActor* Instigator, UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool FireASProjectileFromWeaponAtAimLocation(TSubclassOf<ASoSASProjectileBase> Projectile, AActor* Instigator, const FVector &SocketLocation, UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static EASTeam GetASTeam(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASPlayAnimation(USkeletalMeshComponent* Mesh, UAnimationAsset* Animation, bool bIsLooping);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASApplyRootMotionConstantForce(ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASApplyRootMotionJumpForce(ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve);
	
	static USoSASAbilityBase* CreateASAbilityInstance(TSubclassOf<USoSASAbilityBase> Ability);

private:

	static void ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, int32 StackToApply, float ApplicationTime);
};
