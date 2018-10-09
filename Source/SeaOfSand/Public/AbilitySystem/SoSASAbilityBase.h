// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASAbilityBase.generated.h"

class ASoSWeaponBase;
class USoSASComponent;

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

	UFUNCTION(BlueprintImplementableEvent, Category = "ASAbility")
	void InitializeAbility();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ASAbility")
	bool StartAbility(AActor* Source, ASoSWeaponBase* Weapon);

	UFUNCTION(BlueprintImplementableEvent, Category = "ASAbility")
	void ASReadyComboAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "ASAbility")
	void ASActionComplete();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ASAbility")
	void ASMeleeStart();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ASAbility")
	void ASMeleeEnd();

protected:

	float LastTimeActivated;

	float LastChargeRemainder;

	int32 CurrentCharges;

	bool bComboReady;

	USoSASComponent* OwningASComp;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility", meta = (ClampMin = "0.001", UIMin = "0.001"))
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility")
	float Cost;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility")
	bool bHasCharges;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility", meta = (ClampMin = "0.001", UIMin = "0.001", EditCondition = "bHasCharges"))
	float ChargeTime;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility", meta = (ClampMin = "1", UIMin = "1", EditCondition = "bHasCharges"))
	int32 MaxCharges;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility")
	EASResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly, Category = "ASAbility")
	UTexture2D* AbilityIcon;

public:

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	float GetLastTimeActivated() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	float GetLastChargeRemainder() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	float GetCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	float GetCost() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	bool GetHasCharges() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	float GetChargeTime() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	bool GetComboReady() const;  

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	int32 GetMaxCharges() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	int32 GetCurrentCharges() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	UTexture2D* GetAbilityIcon() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	EASResourceType GetResourceType() const;

	UFUNCTION(BlueprintCallable, Category = "ASAbility")
	USoSASComponent* GetOwningASComp() const;

	void SetLastTimeActivated(float NewTime);

	void SetLastChargeRemainder(float NewRemainder);

	void SetCurrentCharges(int32 Charges);

	void SetComboReady(bool bNewComboReady);

	void SetOwningASComp(USoSASComponent* ASComp);
};