// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASAbilityBase.generated.h"

UENUM()
enum class EASResourceType : uint8
{
	Energy,
	Health
};

UCLASS(BlueprintType, Blueprintable)
class SEAOFSAND_API USoSASAbilityBase : public UObject
{
	GENERATED_BODY()

protected:

	USoSASAbilityBase();

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	bool StartAbility(AActor* CurrentTarget, AActor* Instigator, USkeletalMeshComponent* WeaponMesh, FName SocketName, float TimeSeconds, UWorld* World);

private:

	float LastTimeActivated;

	float LastChargeRemainder;

	int32 CurrentCharges;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "0.001", UIMin = "0.001"))
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Cost;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bHasCharges;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "0.001", UIMin = "0.001", EditCondition = "bHasCharges"))
	float ChargeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "1", UIMin = "1", EditCondition = "bHasCharges"))
	int32 MaxCharges;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EASResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UTexture2D* AbilityIcon;

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetLastTimeActivated() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetLastChargeRemainder() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCost() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool GetbHasCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetChargeTime() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	int32 GetMaxCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	int32 GetCurrentCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	UTexture2D* GetAbilityIcon() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	EASResourceType GetResourceType() const;

	void SetLastTimeActivated(float NewTime);

	void SetLastChargeRemainder(float NewRemainder);

	void SetCurrentCharges(int32 Charges);
};