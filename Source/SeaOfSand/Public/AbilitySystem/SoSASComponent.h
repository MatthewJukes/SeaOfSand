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


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectUpdate, USoSASComponent*, ASComp, const FASEffectData&, Effect, EASEffectUpdateEventType, EventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagUpdate, const EASTag&, Tag, EASTagUpdateEventType, EventType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSASComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USoSASComponent();

protected:

	virtual void BeginPlay() override;

public:

	void LoopOverCurrentASEffectsArray();

	void AddASEffectToArray(FASEffectData& EffectToAdd);

	void AddValueToASAttributeBaseValues(EASAttributeName Attribute, float Value);

	bool UseASAbility(USoSASAbilityBase* Ability);

	void DamageCalculation(float Damage, FASDamageType& DamageType);

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void ASActionStart();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void ASReadyComboAction();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	void ASActionComplete();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectUpdate OnEffectUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTagUpdate OnTagUpdate;

protected:

	TArray<FASEffectData> CurrentASEffects;

	TArray<EASTag> CurrentASEffectTags;

	FASAttributeData ASAttributeBaseValues;

	FASAttributeData ASAttributeTotalValues;

	FASAttributeData ASAttributeTempAdditiveValues;

	FASAttributeData ASAttributeTempMultiplierValues;

	EASOwnerState OwnerState;

	USoSASAbilityBase* LastAbilityToStartMontage;

	ASoSGameModeBase* SoSGameMode;

	ACharacter* ComponentOwner;

	USoSInventoryComponent* OwnerInventory;

	FName WeaponProjectileOriginSocketName;

	FVector* AimHitLocation;

	UDataTable* DamageTypeDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	EASTeam Team;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float HealthMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float ArmourMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float EnergyMaxStartValue;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Stats")
	float SpeedStartValue;

	void CheckASEffectStatus(FASEffectData& Effect);

	void HandleASEffectAttributeModifierValue(FASEffectData& Effect, FASEffectAttributeModifierModule& Module, bool bUseTotalValue);

	UFUNCTION()
	void TagUpdate(const EASTag& Tag, EASTagUpdateEventType EventType);

	void AddValueToASAttributeData(FASAttributeData& AttributeData, EASAttributeName Attribute, float Value);

	void MultiplyASAttributeDataByValue(FASAttributeData& AttributeData, EASAttributeName Attribute, float Value);

	void CalculateASAttributeTotalValues();

	void RemoveASEffectFromArrayByIndex(int32 Index);

	void RemoveASEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove);

	void EndASEffect(FASEffectData& EffectToEnd);

	bool ASAbilityCheckCooldownAndCharges(USoSASAbilityBase* AbilityToCheck);

	bool ASAbilityHandleResource(EASResourceType Type, float Cost);

////////////////////////////////////////////////
// Getters and Setters

public:
	
	float GetASAttributeDataValue(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	float GetASAttributeTotalValue(EASAttributeName AttributeToGet) const;

	TArray<FASEffectData>& GetCurrentEffectsArray();

	TArray<EASTag>& GetCurrentASEffectTags();

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	EASOwnerState GetASOwnerState() const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	USoSASAbilityBase* GetLastAbilityToStartMontage() const;

	UFUNCTION(BlueprintCallable, Category = "ASComponent")
	USoSInventoryComponent* GetOwnerInventory() const;

	EASTeam GetASTeam() const;

	FVector* GetAimHitLocation() const;

	void SetASOwnerState(EASOwnerState NewState);

	void SetOwnerInventory(USoSInventoryComponent* InventoryComp);

	void SetWeaponProjectileOriginSocketName(FName SocketName);

	void SetAimHitLocation(FVector* AimHitLocationReference);

	void SetLastAbilityToStartMontage(USoSASAbilityBase* Ability);
};
