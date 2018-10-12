// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASComponent.h"
#include "SoSGameModeBase.h"
#include "SoSASAbilityBase.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "SoSWeaponBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSASComponent::USoSASComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthMaxStartValue = 100;
	ArmourMaxStartValue = 0;
	EnergyMaxStartValue = 100;
	SpeedStartValue = 400;

	CurrentASEffects.Reserve(10);

	ConstructorHelpers::FObjectFinder<UDataTable> DT_DamageTypes(TEXT("DataTable'/Game/AbilitySystem/Data/DT_ASDamageTypes.DT_ASDamageTypes'"));
	DamageTypeDataTable = DT_DamageTypes.Object;
}


// Called when the game starts
void USoSASComponent::BeginPlay()
{
	Super::BeginPlay();

	ASAttributeBaseValues.HealthMaxValue = HealthMaxStartValue;
	ASAttributeBaseValues.HealthCurrentValue = HealthMaxStartValue;
	ASAttributeBaseValues.ArmourMaxValue = ArmourMaxStartValue;
	ASAttributeBaseValues.ArmourCurrentValue = ArmourMaxStartValue;
	ASAttributeBaseValues.EnergyMaxValue = EnergyMaxStartValue;
	ASAttributeBaseValues.EnergyCurrentValue = EnergyMaxStartValue;
	ASAttributeBaseValues.SpeedValue = SpeedStartValue;

	ASAttributeTempMultiplierValues.HealthMaxValue = 1;
	ASAttributeTempMultiplierValues.HealthCurrentValue = 1;
	ASAttributeTempMultiplierValues.ArmourMaxValue = 1;
	ASAttributeTempMultiplierValues.ArmourCurrentValue = 1;
	ASAttributeTempMultiplierValues.EnergyMaxValue = 1;
	ASAttributeTempMultiplierValues.EnergyCurrentValue = 1;
	ASAttributeTempMultiplierValues.SpeedValue = 1;

	SoSGameMode = Cast<ASoSGameModeBase>(GetWorld()->GetAuthGameMode());
	SoSGameMode->AddASComponentToArray(this);

	ComponentOwner = Cast<ACharacter>(GetOwner());

	OnTagUpdate.AddDynamic(this, &USoSASComponent::TagUpdate);
}


void USoSASComponent::LoopOverCurrentASEffectsArray()
{
	int32 Index = 0;
	TArray<int32> EffectIndexToRemove;
	for (FASEffectData& Effect : CurrentASEffects)
	{
		CheckASEffectStatus(Effect);
		if (Effect.bExpired)
		{
			EffectIndexToRemove.Add(Index);
		}
		Index++;
	}
	RemoveASEffectFromArrayByIndexArray(EffectIndexToRemove);

	CalculateASAttributeTotalValues();
	//UE_LOG(LogTemp, Warning, TEXT("ASComp: %s, MaxHealth: %f"), *this->GetFName().ToString(), ASAttributeTotalValues.HealthMaxValue);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), ASAttributeTotalValues.HealthCurrentValue);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentHealthBase: %f"), ASAttributeBaseValues.HealthCurrentValue);
	//UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), ASAttributeTotalValues.SpeedValue);
}


void USoSASComponent::CheckASEffectStatus(FASEffectData& Effect)
{
	// Check if effect should expire
	float EffectElapsedTime = GetWorld()->GetTimeSeconds() - Effect.EffectStartTime;
	if (EffectElapsedTime >= Effect.EffectDuration)
	{
		EndASEffect(Effect);
		return;
	}

	// Check if effect should tick
	float TimeElapsedSinceLastTick = GetWorld()->GetTimeSeconds() - Effect.LastTickTime;
	if (TimeElapsedSinceLastTick >= Effect.TickRate)
	{
		Effect.LastTickTime = GetWorld()->GetTimeSeconds();
		
		for (FASEffectAttributeModifierModule& Module : Effect.AttributeModifierModules)
		{
			HandleASEffectAttributeModifierValue(Effect, Module, false);
		}

		for (FASEffectAbilityModule& Module : Effect.AbilityModules)
		{
			HandleASEffectAbility(Effect, Module);
		}

		for (FASEffectDamageModule& Module : Effect.DamageModules)
		{
			FString RowString;
			DamageCalculation(Module.DamageValue * Effect.CurrentStacks, Module.DamageType);
		}

		Effect.bFirstTick = false;
	}
}


void USoSASComponent::HandleASEffectAttributeModifierValue(FASEffectData& Effect, FASEffectAttributeModifierModule& Module, bool bUseTotalValue)
{ 
	float PosNegMultiplier = Module.ModifierValue < 0 ? -1 : 1;
	float NewValue = Effect.bNonTicking ? FMath::Abs(Module.ModifierValue) * Effect.NewStacks * PosNegMultiplier : FMath::Abs(Module.ModifierValue) * Effect.CurrentStacks * PosNegMultiplier;
	NewValue = bUseTotalValue ? Module.TotalValue : NewValue;
	Module.TotalValue += NewValue;

	if (Module.bTemporaryModifier)
	{
		switch (Module.ModifierValueType)
		{
		case EASEffectValueType::Additive:
			AddValueToASAttributeData(ASAttributeTempAdditiveValues, Module.AttributeToEffect, NewValue);
			break;
		case EASEffectValueType::Multiplicative:
			NewValue *= 0.01f;
			AddValueToASAttributeData(ASAttributeTempMultiplierValues, Module.AttributeToEffect, NewValue);
			break;
		case EASEffectValueType::Subtractive:
			NewValue *= 0.01f;
			AddValueToASAttributeData(ASAttributeTempMultiplierValues, Module.AttributeToEffect, -NewValue);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (Module.ModifierValueType)
		{
		case EASEffectValueType::Additive:
			AddValueToASAttributeData(ASAttributeBaseValues, Module.AttributeToEffect, NewValue);
			break;
		case EASEffectValueType::Multiplicative:
			NewValue *= 0.01f;
			MultiplyASAttributeDataByValue(ASAttributeBaseValues, Module.AttributeToEffect, 1 + NewValue);
			break;
		case EASEffectValueType::Subtractive:
			NewValue *= 0.01f;
			MultiplyASAttributeDataByValue(ASAttributeBaseValues, Module.AttributeToEffect, 1 - NewValue);
			break;
		default:
			break;
		}
	}
}


void USoSASComponent::HandleASEffectAbility(FASEffectData& Effect, FASEffectAbilityModule& Module)
{
	if (Module.Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Module Ability is NULL"), *Effect.EffectName.ToString())
		return;
	}

	if (Module.UseAbilityOn == EASEffectAbilityTickType::FirstTick && !Effect.bFirstTick)
	{
		return;
	}

	if (Module.UseAbilityOn == EASEffectAbilityTickType::LastTick)
	{
		float EffectTimeRemaining = Effect.EffectDuration - (GetWorld()->GetTimeSeconds() - Effect.EffectStartTime);
		if (EffectTimeRemaining > Effect.TickRate)
		{
			return;
		}
	}

	USoSInventoryComponent* SourceInventory = Cast<USoSInventoryComponent>(Effect.Source->GetComponentByClass(USoSInventoryComponent::StaticClass()));
	Module.Ability->StartAbility(Effect.Source, SourceInventory->GetCurrentWeapon());
}

void USoSASComponent::TagUpdate(const EASTag& Tag, EASTagUpdateEventType EventType)
{
	switch (EventType)
	{
	case EASTagUpdateEventType::Added:
		for (FASEffectData& Effect : CurrentASEffects)
		{
			if (Effect.EffectBlockedByTags.Contains(Tag))
			{
				EndASEffect(Effect);
			}
		}
		break;
	case EASTagUpdateEventType::Removed:
		break;
	default:
		break;
	}
}


void USoSASComponent::DamageCalculation(float Damage, EASDamageTypeName DamageTypeName)
{
	FASDamageType* DamageType = new FASDamageType;
	DamageType->ArmourPenetration = 0;
	DamageType->ArmourDamage = 50;

	FString ReferenceString = FString("");

	switch (DamageTypeName)
	{
	case EASDamageTypeName::Default:
		DamageType = DamageTypeDataTable->FindRow<FASDamageType>(FName("Default"), ReferenceString);
		break;
	case EASDamageTypeName::Pure:
		DamageType = DamageTypeDataTable->FindRow<FASDamageType>(FName("Pure"), ReferenceString);
		break;
	case EASDamageTypeName::Fire:
		DamageType = DamageTypeDataTable->FindRow<FASDamageType>(FName("Fire"), ReferenceString);
		break;
	default:
		break;
	}

	float MaxRawDamageReductionByArmour = FMath::Max(1.0f, ASAttributeTotalValues.ArmourCurrentValue * 0.1f); // 10% of current armour
	
	// Calculate armour penetration
	float HealthDamage = Damage * (DamageType->ArmourPenetration * 0.01f);
	HealthDamage += (Damage - HealthDamage) - FMath::Min(MaxRawDamageReductionByArmour, (Damage - HealthDamage));

	// Calculate armour damage
	float ArmourDamage = (Damage - HealthDamage) * (DamageType->ArmourDamage * 0.01f);

	AddValueToASAttributeBaseValues(EASAttributeName::HealthCurrent, -HealthDamage);
	AddValueToASAttributeBaseValues(EASAttributeName::ArmourCurrent, -ArmourDamage);
}


void USoSASComponent::AddValueToASAttributeData(FASAttributeData& AttributeData, EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		AttributeData.HealthMaxValue += Value;
		break;
	case EASAttributeName::HealthCurrent:
		AttributeData.HealthCurrentValue += Value;
		break;
	case EASAttributeName::ArmourMax:
		AttributeData.ArmourMaxValue += Value;
		break;
	case EASAttributeName::ArmourCurrent:
		AttributeData.ArmourCurrentValue += Value;
		break;
	case EASAttributeName::EnergyMax:
		AttributeData.EnergyMaxValue += Value;
		break;
	case EASAttributeName::EnergyCurrent:
		AttributeData.EnergyCurrentValue += Value;
		break;
	case EASAttributeName::Speed:
		AttributeData.SpeedValue += Value;
		break;
	default:
		break;
	}
}


void USoSASComponent::MultiplyASAttributeDataByValue(FASAttributeData& AttributeData, EASAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EASAttributeName::HealthMax:
		AttributeData.HealthMaxValue *= Value;
		break;
	case EASAttributeName::HealthCurrent:
		AttributeData.HealthCurrentValue *= Value;
		break;
	case EASAttributeName::ArmourMax:
		AttributeData.ArmourMaxValue *= Value;
		break;
	case EASAttributeName::ArmourCurrent:
		AttributeData.ArmourCurrentValue *= Value;
		break;
	case EASAttributeName::EnergyMax:
		AttributeData.EnergyMaxValue *= Value;
		break;
	case EASAttributeName::EnergyCurrent:
		AttributeData.EnergyCurrentValue *= Value;
		break;
	case EASAttributeName::Speed:
		AttributeData.SpeedValue *= Value;
		break;
	default:
		break;
	}
}


void USoSASComponent::CalculateASAttributeTotalValues()
{
	ASAttributeTotalValues.HealthMaxValue = FMath::Max(1.0f, ASAttributeBaseValues.HealthMaxValue * ASAttributeTempMultiplierValues.HealthMaxValue + ASAttributeTempAdditiveValues.HealthMaxValue);

	ASAttributeTotalValues.HealthCurrentValue = FMath::Clamp(ASAttributeBaseValues.HealthCurrentValue * ASAttributeTempMultiplierValues.HealthCurrentValue + ASAttributeTempAdditiveValues.HealthCurrentValue, 0.0f, ASAttributeTotalValues.HealthMaxValue);
	ASAttributeBaseValues.HealthCurrentValue = FMath::Clamp(ASAttributeBaseValues.HealthCurrentValue, 0.0f, ASAttributeTotalValues.HealthMaxValue + (ASAttributeBaseValues.HealthCurrentValue - (ASAttributeBaseValues.HealthCurrentValue * ASAttributeTempMultiplierValues.HealthCurrentValue + ASAttributeTempAdditiveValues.HealthCurrentValue)));

	ASAttributeTotalValues.ArmourMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.ArmourMaxValue * ASAttributeTempMultiplierValues.ArmourMaxValue + ASAttributeTempAdditiveValues.ArmourMaxValue);

	ASAttributeTotalValues.ArmourCurrentValue = FMath::Clamp(ASAttributeBaseValues.ArmourCurrentValue * ASAttributeTempMultiplierValues.ArmourCurrentValue + ASAttributeTempAdditiveValues.ArmourCurrentValue, 0.0f, ASAttributeTotalValues.ArmourMaxValue);
	ASAttributeBaseValues.ArmourCurrentValue = FMath::Clamp(ASAttributeBaseValues.ArmourCurrentValue, 0.0f, ASAttributeTotalValues.ArmourMaxValue + (ASAttributeBaseValues.ArmourCurrentValue - (ASAttributeBaseValues.ArmourCurrentValue * ASAttributeTempMultiplierValues.ArmourCurrentValue + ASAttributeTempAdditiveValues.ArmourCurrentValue)));

	ASAttributeTotalValues.EnergyMaxValue = FMath::Max(0.0f, ASAttributeBaseValues.EnergyMaxValue * ASAttributeTempMultiplierValues.EnergyMaxValue + ASAttributeTempAdditiveValues.EnergyMaxValue);

	ASAttributeTotalValues.EnergyCurrentValue = FMath::Clamp(ASAttributeBaseValues.EnergyCurrentValue * ASAttributeTempMultiplierValues.EnergyCurrentValue + ASAttributeTempAdditiveValues.EnergyCurrentValue, 0.0f, ASAttributeTotalValues.EnergyMaxValue);
	ASAttributeBaseValues.EnergyCurrentValue = FMath::Clamp(ASAttributeBaseValues.EnergyCurrentValue, 0.0f, ASAttributeTotalValues.EnergyMaxValue + (ASAttributeBaseValues.EnergyCurrentValue - (ASAttributeBaseValues.EnergyCurrentValue * ASAttributeTempMultiplierValues.EnergyCurrentValue + ASAttributeTempAdditiveValues.EnergyCurrentValue)));

	ASAttributeTotalValues.SpeedValue = FMath::Max(0.0f, ASAttributeBaseValues.SpeedValue * ASAttributeTempMultiplierValues.SpeedValue + ASAttributeTempAdditiveValues.SpeedValue);

	if (ComponentOwner == nullptr)
	{
		return;
	}

	if (UCharacterMovementComponent* CharacterMovement = ComponentOwner->GetCharacterMovement())
	{
		CharacterMovement->MaxWalkSpeed = ASAttributeTotalValues.SpeedValue;
	}
}


void USoSASComponent::AddValueToASAttributeBaseValues(EASAttributeName Attribute, float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect Added: %f"), Value)
	AddValueToASAttributeData(ASAttributeBaseValues, Attribute, Value);
}


void USoSASComponent::AddASEffectToArray(FASEffectData& EffectToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect Added: %s"), *EffectToAdd.EffectName.ToString())
	CurrentASEffects.Add(EffectToAdd);
	for (EASTag Tag : EffectToAdd.EffectAppliesTags)
	{
		CurrentASEffectTags.Add(Tag);
		OnTagUpdate.Broadcast(Tag, EASTagUpdateEventType::Added);
	}

	OnEffectUpdate.Broadcast(this, EffectToAdd, EASEffectUpdateEventType::Added);
}


void USoSASComponent::RemoveASEffectFromArrayByIndex(int32 Index)
{
	OnEffectUpdate.Broadcast(this, CurrentASEffects[Index], EASEffectUpdateEventType::Removed);

	UE_LOG(LogTemp, Warning, TEXT("Effect Removed: %s"), *CurrentASEffects[Index].EffectName.ToString())
	CurrentASEffects.RemoveAt(Index);
}


void USoSASComponent::RemoveASEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove)
{
	for (int32 Index = EffectIndexesToRemove.Num()-1; Index >= 0; Index--)
	{
		RemoveASEffectFromArrayByIndex(EffectIndexesToRemove[Index]);
	}
}


void USoSASComponent::EndASEffect(FASEffectData& EffectToEnd)
{
	if (EffectToEnd.bExpired)
	{
		return;
	}

	EffectToEnd.bExpired = true;

	for (FASEffectAttributeModifierModule& Module : EffectToEnd.AttributeModifierModules)
	{
		if (Module.bTemporaryModifier)
		{
			Module.TotalValue = -Module.TotalValue;
			HandleASEffectAttributeModifierValue(EffectToEnd, Module, true);
		}
	}

	for (EASTag Tag : EffectToEnd.EffectAppliesTags)
	{
		CurrentASEffectTags.Remove(Tag);
		OnTagUpdate.Broadcast(Tag, EASTagUpdateEventType::Removed);
	}
}


bool USoSASComponent::UseASAbility(USoSASAbilityBase* Ability)
{
	if (Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Cast is NULL"));
		return false;
	}

	if (OwnerState == EASOwnerState::PerformingAction && !Ability->GetComboReady())
	{
		return false;
	}

	if (!ASAbilityCheckCooldownAndCharges(Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability %s on cooldown"), *Ability->GetName());
		return false;
	}

	if (ASAbilityHandleResource(Ability->GetResourceType(), Ability->GetCost()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Cast: %s"), *Ability->GetName());

		UWorld* World = GetWorld();
		Ability->SetLastTimeActivated(World->GetTimeSeconds());
		return Ability->StartAbility(GetOwner(), OwnerInventory->GetCurrentWeapon());
	}

	return false;
}


void USoSASComponent::ASActionStart()
{
	ASoSPlayerCharacter* Player = Cast<ASoSPlayerCharacter>(ComponentOwner); // TODO create a SoSCharacterBase class
	Player->SprintEnd();
	Player->AimEnd();

	LastAbilityToStartMontage->SetComboReady(false);

	OwnerState = EASOwnerState::PerformingAction;
}


void USoSASComponent::ASReadyComboAction()
{
	LastAbilityToStartMontage->SetComboReady(true);
	LastAbilityToStartMontage->ASReadyComboAction();
}


void USoSASComponent::ASActionComplete()
{
	LastAbilityToStartMontage->SetComboReady(false);
	LastAbilityToStartMontage->ASActionComplete();

	OwnerState = EASOwnerState::Normal;
}


bool USoSASComponent::ASAbilityCheckCooldownAndCharges(USoSASAbilityBase* AbilityToCheck)
{
	if (GetWorld()->GetTimeSeconds() - AbilityToCheck->GetLastTimeActivated() < AbilityToCheck->GetCooldown())
	{
		return false;
	}

	if (!AbilityToCheck->GetHasCharges())
	{
		return true;
	}

	float NewCharges = ((GetWorld()->GetTimeSeconds() - AbilityToCheck->GetLastTimeActivated()) / AbilityToCheck->GetChargeTime()) + AbilityToCheck->GetLastChargeRemainder();
	NewCharges = FMath::Clamp(NewCharges, 0.0f, float(AbilityToCheck->GetMaxCharges() - AbilityToCheck->GetCurrentCharges()));

	AbilityToCheck->SetCurrentCharges(AbilityToCheck->GetCurrentCharges() + FMath::TruncToInt(NewCharges));

	if (AbilityToCheck->GetCurrentCharges() == 0)
	{
		return false;
	}

	AbilityToCheck->SetLastChargeRemainder(NewCharges - FMath::TruncToInt(NewCharges));
	AbilityToCheck->SetCurrentCharges(AbilityToCheck->GetCurrentCharges() - 1);
	return true;
}


bool USoSASComponent::ASAbilityHandleResource(EASResourceType Type, float Cost)
{
	switch (Type)
	{
	case EASResourceType::Energy:
		if (ASAttributeTotalValues.EnergyCurrentValue < Cost)
		{
			return false;
		}
		AddValueToASAttributeData(ASAttributeBaseValues, EASAttributeName::EnergyCurrent, -Cost);
		break;
	case EASResourceType::Health:
		if (ASAttributeTotalValues.HealthCurrentValue+1 < Cost)
		{
			return false;
		}
		AddValueToASAttributeData(ASAttributeBaseValues, EASAttributeName::HealthCurrent, -Cost);
		break;
	default:
		break;
	}

	return true;
}


////////////////////////////////////////////////
// Getters and Setters


float USoSASComponent::GetASAttributeDataValue(FASAttributeData* AttributeData, EASAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EASAttributeName::HealthMax:
		return AttributeData->HealthMaxValue;
		break;
	case EASAttributeName::HealthCurrent:
		return AttributeData->HealthCurrentValue;
		break;
	case EASAttributeName::ArmourMax:
		return AttributeData->ArmourMaxValue;
		break;
	case EASAttributeName::ArmourCurrent:
		return AttributeData->ArmourCurrentValue;
		break;
	case EASAttributeName::EnergyMax:
		return AttributeData->EnergyMaxValue;
		break;
	case EASAttributeName::EnergyCurrent:
		return AttributeData->EnergyCurrentValue;
		break;
	case EASAttributeName::Speed:
		return AttributeData->SpeedValue;
		break;
	default:
		return -1;
		break;
	}
}


float USoSASComponent::GetASAttributeTotalValue(EASAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EASAttributeName::HealthMax:
		return ASAttributeTotalValues.HealthMaxValue;
		break;
	case EASAttributeName::HealthCurrent:
		return ASAttributeTotalValues.HealthCurrentValue;
		break;
	case EASAttributeName::ArmourMax:
		return ASAttributeTotalValues.ArmourMaxValue;
		break;
	case EASAttributeName::ArmourCurrent:
		return ASAttributeTotalValues.ArmourCurrentValue;
		break;
	case EASAttributeName::EnergyMax:
		return ASAttributeTotalValues.EnergyMaxValue;
		break;
	case EASAttributeName::EnergyCurrent:
		return ASAttributeTotalValues.EnergyCurrentValue;
		break;
	case EASAttributeName::Speed:
		return ASAttributeTotalValues.SpeedValue;
		break;
	default:
		return -1;
		break;
	}
}


TArray<FASEffectData>& USoSASComponent::GetCurrentEffectsArray()
{
	return CurrentASEffects;
}


TArray<EASTag>& USoSASComponent::GetCurrentASEffectTags()
{
	return CurrentASEffectTags;
}


EASOwnerState USoSASComponent::GetASOwnerState() const
{
	return OwnerState;
}


USoSASAbilityBase* USoSASComponent::GetLastAbilityToStartMontage() const
{
	return LastAbilityToStartMontage;
}


USoSInventoryComponent* USoSASComponent::GetOwnerInventory() const
{
	return OwnerInventory;
}

EASTeam USoSASComponent::GetASTeam() const
{
	return Team;
}


FVector* USoSASComponent::GetAimHitLocation() const
{
	return AimHitLocation;
}


void USoSASComponent::SetASOwnerState(EASOwnerState NewState)
{
	OwnerState = NewState;
}


void USoSASComponent::SetOwnerInventory(USoSInventoryComponent* InventoryComp)
{
	OwnerInventory = InventoryComp;
}


void USoSASComponent::SetWeaponProjectileOriginSocketName(FName SocketName)
{
	WeaponProjectileOriginSocketName = SocketName;
}


void USoSASComponent::SetAimHitLocation(FVector* AimHitLocationReference)
{
	AimHitLocation = AimHitLocationReference;
}


void USoSASComponent::SetLastAbilityToStartMontage(USoSASAbilityBase* Ability)
{
	LastAbilityToStartMontage = Ability;
}

