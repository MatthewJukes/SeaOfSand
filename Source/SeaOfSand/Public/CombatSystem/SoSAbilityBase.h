// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSAbilityBase.generated.h"


class USoSCombatComponent;


UENUM()
enum class EAbilityResourceType : uint8
{
	Energy,
	Health
};


UENUM()
enum class EAbilityCastType : uint8
{
	Default,  // Ability start and release on key down and release. Cannot be cast while preforming action
	Instant, //  Ability start and release on key down and release. Can be cast anytime but cannot have an animation
	Aimed, // Enter targeting mode upon key release. Targeting keys control ability start and release
	AimedCharge // Ability start on key down, enter targeting mode upon key release. Targeting keys then control ability release
};


UENUM()
enum class EAbilityTargetingShape : uint8
{
	Tube,
	Sphere
};


UCLASS(BlueprintType, Blueprintable)
class SEAOFSAND_API USoSAbilityBase : public UObject
{
	GENERATED_BODY()

protected:

	USoSAbilityBase();

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void InitializeAbility();
	virtual void InitializeAbility_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	bool StartAbility(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue);
	virtual bool StartAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue);

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	bool ReleaseAbility(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue);
	virtual bool ReleaseAbility_Implementation(USoSCombatComponent* SourceCombatComp, float ClassSpecificFloatValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void ReadyCombo();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void EndCombo();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void ActionComplete();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void HitboxActivate();
	virtual void HitboxActivate_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void HitboxDeactivate();
	virtual void HitboxDeactivate_Implementation();

protected:

	float LastTimeActivated;

	float LastChargeRemainder;

	int32 CurrentCharges;

	bool bComboReady;

	USoSCombatComponent* OwningCombatComp;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityCastType CastType;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Cost;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "0.001", UIMin = "0.001"))
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bHasCharges;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "0.001", UIMin = "0.001", EditCondition = "bHasCharges"))
	float ChargeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "1", UIMin = "1", EditCondition = "bHasCharges"))
	int32 MaxCharges;

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Visual")
	UTexture2D* AbilityIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Targeting")
	float TargetRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Targeting")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Targeting")
	bool bSnapToGround;

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Targeting")
	EAbilityTargetingShape TargetingShape;

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	EAbilityCastType GetCastType();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetLastTimeActivated() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetLastChargeRemainder() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCost() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool GetHasCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetChargeTime() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool GetComboReady() const;  

	UFUNCTION(BlueprintCallable, Category = "Ability")
	int32 GetMaxCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	int32 GetCurrentCharges() const;

	UFUNCTION(BlueprintCallable, Category = "Ability | Visual")
	UTexture2D* GetAbilityIcon() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	EAbilityResourceType GetResourceType() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	USoSCombatComponent* GetOwningCombatComp() const;

	UFUNCTION(BlueprintCallable, Category = "Ability | Targeting")
	float GetTargetRadius() const;

	UFUNCTION(BlueprintCallable, Category = "Ability | Targeting")
	float GetMaxRange();

	UFUNCTION(BlueprintCallable, Category = "Ability | Targeting")
	bool GetSnapToGround();

	EAbilityTargetingShape GetTargetingShape();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetComboReady(bool bNewComboReady);

	void SetLastTimeActivated(float NewTime);

	void SetLastChargeRemainder(float NewRemainder);

	void SetCurrentCharges(int32 Charges);

	void SetOwningCombatComp(USoSCombatComponent* CombatComp);
};