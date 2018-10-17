// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffectData.h"
#include "Components/ActorComponent.h"
#include "SoSASComponent.generated.h"

class USoSInventoryComponent;
class USoSASAbilityBase;
class ASoSGameModeBase;
class ASoSWeaponBase;
class ACharacter;
enum class EASResourceType : uint8;


UENUM(BlueprintType)
enum class EASOwnerState : uint8
{
	Normal,
	Sprinting,
	Aiming,
	PerformingAction,
	Dead
};


UENUM(BlueprintType)
enum class EASEffectUpdateEventType : uint8
{
	Added,
	Removed,
	Reapplied
};


UENUM(BlueprintType)
enum class EASTagUpdateEventType : uint8
{
	Added,
	Removed
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectUpdate, USoSASComponent*, ASComp, const FEffectData&, Effect, EASEffectUpdateEventType, EventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagUpdate, const EAbilityTag&, Tag, EASTagUpdateEventType, EventType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSASComponent();

protected:

	virtual void BeginPlay() override;

public:

	void LoopOverCurrentEffectsArray();

	void AddEffectToArray(FEffectData& EffectToAdd);

	void AddValueToAttributeBaseValues(EAttributeName Attribute, float Value);

	bool UseAbility(USoSASAbilityBase* Ability);

	void DamageCalculation(float Damage, ESoSDamageTypeName DamageTypeName);

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void AbilityActionStart();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void AbilityReadyComboAction();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void AbilityActionComplete();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectUpdate OnEffectUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTagUpdate OnTagUpdate;

protected:

	TArray<FEffectData> CurrentEffects;

	TArray<EAbilityTag> CurrentEffectTags;

	FAttributeData AttributeBaseValues;

	FAttributeData AttributeTotalValues;

	FAttributeData AttributeTempAdditiveValues;

	FAttributeData AttributeTempMultiplierValues;

	EASOwnerState OwnerState;

	USoSASAbilityBase* LastAbilityToStartMontage;

	ASoSGameModeBase* SoSGameMode;

	ACharacter* ComponentOwner;

	USoSInventoryComponent* OwnerInventory;

	FName WeaponProjectileOriginSocketName;

	FVector* AimHitLocation;

	UDataTable* DamageTypeDataTable;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	ESoSTeam Team;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float HealthMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float ArmourMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float EnergyMaxStartValue;

	UPROPERTY(EditAnywhere, Category = "Character | Stats")
	float SpeedStartValue;

	void CheckEffectStatus(FEffectData& Effect);

	void HandleEffectAttributeModifierValue(FEffectData& Effect, FEffectAttributeModifierModule& Module, bool bUseTotalValue);

	void HandleEffectAbility(FEffectData& Effect, FEffectAbilityModule& Module);

	UFUNCTION()
	void TagUpdate(const EAbilityTag& Tag, EASTagUpdateEventType EventType);

	void AddValueToAttributeData(FAttributeData& AttributeData, EAttributeName Attribute, float Value);

	void MultiplyAttributeDataByValue(FAttributeData& AttributeData, EAttributeName Attribute, float Value);

	void CalculateAttributeTotalValues();

	void RemoveEffectFromArrayByIndex(int32 Index);

	void RemoveEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove);

	void EndEffect(FEffectData& EffectToEnd);

	bool AbilityCheckCooldownAndCharges(USoSASAbilityBase* AbilityToCheck);

	bool AbilityHandleResource(EASResourceType Type, float Cost);

////////////////////////////////////////////////
// Getters and Setters

public:
	
	float GetAttributeDataValue(FAttributeData* AttributeData, EAttributeName AttributeToGet) const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	float GetAttributeTotalValue(EAttributeName AttributeToGet) const;

	TArray<FEffectData>& GetCurrentEffectsArray();

	TArray<EAbilityTag>& GetCurrentEffectTags();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	EASOwnerState GetOwnerState() const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	USoSASAbilityBase* GetLastAbilityToStartMontage() const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	USoSInventoryComponent* GetOwnerInventory() const;

	ESoSTeam GetTeam() const;

	FVector* GetAimHitLocation() const;

	void SetOwnerState(EASOwnerState NewState);

	void SetOwnerInventory(USoSInventoryComponent* InventoryComp);

	void SetWeaponProjectileOriginSocketName(FName SocketName);

	void SetAimHitLocation(FVector* AimHitLocationReference);

	void SetLastAbilityToStartMontage(USoSASAbilityBase* Ability);
};
