// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASEffectData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

class ASoSASProjectileBase;
class UWorld;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, float EffectDuration, float ApplicationTime);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool CheckIfTargetHasASEffectActive(FName EffectName, AActor* Target, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ASWeaponTrace(AActor* Instigator, FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool FireASProjectile(TSubclassOf<ASoSASProjectileBase> Projectile, const FTransform &SpawnTransform, AActor* Instigator, UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static EASTeam GetASTeam(const AActor* Target);

private:

	static void ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, float ApplicationTime);
};
