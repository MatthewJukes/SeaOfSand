// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASEffectData.h"
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

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ApplyASEffectToTarget(const AActor* Target, const AActor* Source, UPARAM(ref) FASEffectData& EffectToApply, int32 StackToApply, float EffectDuration);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool CheckIfTargetHasASEffectActive(const AActor* Target, FName EffectName, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASDamageTarget(const AActor* Target, const AActor* Source, float Value);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static FVector ASGetAimHitLocation(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASWeaponTrace(const AActor* Source, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool FireASProjectile(AActor* Source, TSubclassOf<ASoSASProjectileBase> Projectile, const FTransform &SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool FireASProjectileFromWeaponAtAimLocation(AActor* Source, TSubclassOf<ASoSASProjectileBase> Projectile, const FVector &SocketLocation);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASMeleeHitCheck(const AActor* Source, AActor* Target, UPARAM(ref) TArray<AActor*>& PreviouslyHitActors);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static EASTeam GetASTeam(const AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASApplyRootMotionConstantForce(const ACharacter* TargetCharacter, FVector Direction, float Strength, float Duration, bool bIsAdditive, UCurveFloat* StrengthOverTime, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASApplyRootMotionJumpForce(const ACharacter* TargetCharacter, const FRotator &Rotation, float Distance, float Height, float Duration, bool bFinishOnLanded, ERootMotionFinishVelocityMode VelocityOnFinishMode, const FVector &SetVelocityOnFinish, float ClampVelocityOnFinish, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve);
	
	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASPlayAnimMontage(USoSASAbilityBase* SourceAbility, ACharacter* Target, UAnimMontage* AnimMontage, float PlayRate = 1.0f, FName StartSectionName = "None");

	static USoSASAbilityBase* CreateASAbilityInstance(TSubclassOf<USoSASAbilityBase> Ability, USoSASComponent* OwningASComp);

private:

	static void ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, int32 StackToApply, float ApplicationTime);

	UFUNCTION(meta = (WorldContext = "Instigator"), Category = "ASTasks")
	static UWorld* ASGetWorldFromContextObject(const UObject* WorldContextObject);
};
