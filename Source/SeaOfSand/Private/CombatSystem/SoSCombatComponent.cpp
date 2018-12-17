// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSCombatComponent.h"
#include "SoSGameModeBase.h"
#include "SoSAbilityBase.h"
#include "SoSCharacterBase.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "SoSWeaponBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USoSCombatComponent::USoSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthMaxStartValue = 100;
	ArmourMaxStartValue = 0;
	EnergyMaxStartValue = 100;
	SpeedStartValue = 400;

	CurrentEffects.Reserve(10);

	ConstructorHelpers::FObjectFinder<UDataTable> DT_DamageTypes(TEXT("DataTable'/Game/CombatSystem/Data/DataTable_DamageTypes.DataTable_DamageTypes'"));
	DamageTypeDataTable = DT_DamageTypes.Object;
}


// Called when the game starts
void USoSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AttributeBaseValues.HealthMaxValue = HealthMaxStartValue;
	AttributeBaseValues.HealthCurrentValue = HealthMaxStartValue;
	AttributeBaseValues.ArmourMaxValue = ArmourMaxStartValue;
	AttributeBaseValues.ArmourCurrentValue = ArmourMaxStartValue;
	AttributeBaseValues.EnergyMaxValue = EnergyMaxStartValue;
	AttributeBaseValues.EnergyCurrentValue = 0;
	AttributeBaseValues.SpeedValue = SpeedStartValue;

	AttributeTempMultiplierValues.HealthMaxValue = 1;
	AttributeTempMultiplierValues.HealthCurrentValue = 1;
	AttributeTempMultiplierValues.ArmourMaxValue = 1;
	AttributeTempMultiplierValues.ArmourCurrentValue = 1;
	AttributeTempMultiplierValues.EnergyMaxValue = 1;
	AttributeTempMultiplierValues.EnergyCurrentValue = 1;
	AttributeTempMultiplierValues.SpeedValue = 1;

	ASoSGameModeBase* SoSGameMode = Cast<ASoSGameModeBase>(GetWorld()->GetAuthGameMode());
	SoSGameMode->AddCombatComponentToArray(this);

	OwningCharacter = Cast<ASoSCharacterBase>(GetOwner());

	OnTagUpdate.AddDynamic(this, &USoSCombatComponent::TagUpdate);
}


void USoSCombatComponent::LoopOverCurrentEffectsArray()
{
	int32 Index = 0;
	TArray<int32> EffectIndexToRemove;
	for (FEffectData& Effect : CurrentEffects)
	{
		CheckEffectStatus(Effect);
		if (Effect.bExpired)
		{
			EffectIndexToRemove.Add(Index);
		}
		Index++;
	}
	RemoveEffectFromArrayByIndexArray(EffectIndexToRemove);

	CalculateAttributeTotalValues();
}


void USoSCombatComponent::CheckEffectStatus(FEffectData& Effect)
{
	// Check if On Event Ability has hit max triggers
	for (FEffectOnCombatEventAbilityModule& Module : Effect.OnEventAbilityModules)
	{
		HandleEffectOnEventAbility(Effect, Module);
	}

	// Check if effect should expire
	float EffectElapsedTime = GetWorld()->GetTimeSeconds() - Effect.EffectStartTime;
	if (EffectElapsedTime >= Effect.EffectDuration)
	{
		EndEffect(Effect);
		return;
	}

	// Check if effect should tick
	float TimeElapsedSinceLastTick = GetWorld()->GetTimeSeconds() - Effect.LastTickTime;
	if (TimeElapsedSinceLastTick >= Effect.TickRate)
	{
		Effect.LastTickTime = GetWorld()->GetTimeSeconds();
		
		for (FEffectAttributeModifierModule& Module : Effect.AttributeModifierModules)
		{
			HandleEffectAttributeModifierValue(Effect, Module, false);
		}

		for (FEffectOnTickAbilityModule& Module : Effect.OnTickAbilityModules)
		{
			HandleEffectOnTickAbility(Effect, Module);
		}

		for (FEffectDamageModule& Module : Effect.DamageModules)
		{
			FString RowString;
			float HealthDamage;
			float ArmourDamage;
			DamageCalculation(Module.DamageValue * Effect.CurrentStacks, HealthDamage, ArmourDamage, Module.DamageType);
			OnDamageReceived.Broadcast(Effect.SourceCombatComp, Module.DamageValue * Effect.CurrentStacks, HealthDamage, ArmourDamage);

			if (Effect.SourceCombatComp != nullptr)
			{
				Effect.SourceCombatComp->OnDamageDealt.Broadcast(this, Module.DamageValue * Effect.CurrentStacks, HealthDamage, ArmourDamage);
			}
		}

		Effect.bFirstTick = false;
	}
}


void USoSCombatComponent::HandleEffectAttributeModifierValue(FEffectData& Effect, FEffectAttributeModifierModule& Module, bool bUseTotalValue)
{ 
	float PosNegMultiplier = Module.ModifierValue < 0 ? -1 : 1;
	float NewValue = Effect.bNonTicking ? FMath::Abs(Module.ModifierValue) * Effect.NewStacks * PosNegMultiplier : FMath::Abs(Module.ModifierValue) * Effect.CurrentStacks * PosNegMultiplier;
	NewValue = bUseTotalValue ? Module.TotalValue : NewValue;
	Module.TotalValue += NewValue;

	if (Module.bTemporaryModifier)
	{
		switch (Module.ModifierValueType)
		{
		case EEffectValueType::Additive:
			AddValueToAttributeData(AttributeTempAdditiveValues, Module.AttributeToEffect, NewValue);
			break;
		case EEffectValueType::Multiplicative:
			NewValue *= 0.01f;
			AddValueToAttributeData(AttributeTempMultiplierValues, Module.AttributeToEffect, NewValue);
			break;
		case EEffectValueType::Subtractive:
			NewValue *= 0.01f;
			AddValueToAttributeData(AttributeTempMultiplierValues, Module.AttributeToEffect, -NewValue);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (Module.ModifierValueType)
		{
		case EEffectValueType::Additive:
			AddValueToAttributeData(AttributeBaseValues, Module.AttributeToEffect, NewValue);
			break;
		case EEffectValueType::Multiplicative:
			NewValue *= 0.01f;
			MultiplyAttributeDataByValue(AttributeBaseValues, Module.AttributeToEffect, 1 + NewValue);
			break;
		case EEffectValueType::Subtractive:
			NewValue *= 0.01f;
			MultiplyAttributeDataByValue(AttributeBaseValues, Module.AttributeToEffect, 1 - NewValue);
			break;
		default:
			break;
		}
	}
}


void USoSCombatComponent::HandleEffectOnTickAbility(FEffectData& Effect, FEffectOnTickAbilityModule& Module)
{
	if (Module.Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Module Ability is NULL"), *Effect.EffectName.ToString())
		return;
	}

	if (Module.AbilityTickType == EEffectAbilityTickType::FirstTick && !Effect.bFirstTick)
	{
		return;
	}

	if (Module.AbilityTickType == EEffectAbilityTickType::LastTick)
	{
		float EffectTimeRemaining = Effect.EffectDuration - (GetWorld()->GetTimeSeconds() - Effect.EffectStartTime);
		if (EffectTimeRemaining > Effect.TickRate)
		{
			return;
		}
	}

	ASoSPlayerCharacter* PlayerCharacter = Cast<ASoSPlayerCharacter>(Effect.SourceCombatComp->GetOwningCharacter());
	float ClassSpecificFloat = 0;
	if (PlayerCharacter != nullptr)
	{
		ClassSpecificFloat = PlayerCharacter->GetClassSpecificFloat();
	}
	Module.Ability->StartAbility(Effect.SourceCombatComp, ClassSpecificFloat);
}


void USoSCombatComponent::HandleEffectOnEventAbility(FEffectData& Effect, FEffectOnCombatEventAbilityModule& EffectModule)
{
	if (EffectModule.OnCombatEventAbility->GetRemainingTriggers() == 0)
	{
		EndEffect(Effect);
	}
}


void USoSCombatComponent::TagUpdate(const EAbilityTag& Tag, ETagUpdateEventType EventType)
{
	switch (EventType)
	{
	case ETagUpdateEventType::Added:
		for (FEffectData& Effect : CurrentEffects)
		{
			if (Effect.EffectBlockedByTags.Contains(Tag))
			{
				EndEffect(Effect);
			}
		}
		break;
	case ETagUpdateEventType::Removed:
		break;
	default:
		break;
	}
}


void USoSCombatComponent::DamageCalculation(float BaseDamage, float &OutHealthDamage, float &OutArmourDamage, ESoSDamageTypeName DamageTypeName)
{
	FSoSDamageType* DamageType = new FSoSDamageType;
	DamageType->ArmourPenetration = 0;
	DamageType->ArmourDamage = 50;

	FString ReferenceString = FString("");

	switch (DamageTypeName)
	{
	case ESoSDamageTypeName::Default:
		DamageType = DamageTypeDataTable->FindRow<FSoSDamageType>(FName("Default"), ReferenceString);
		break;
	case ESoSDamageTypeName::Pure:
		DamageType = DamageTypeDataTable->FindRow<FSoSDamageType>(FName("Pure"), ReferenceString);
		break;
	case ESoSDamageTypeName::Fire:
		DamageType = DamageTypeDataTable->FindRow<FSoSDamageType>(FName("Fire"), ReferenceString);
		break;
	default:
		break;
	}
	
	// Calculate armour penetration
	float HealthDamageFromArPen = BaseDamage * DamageType->ArmourPenetration;

	// Calculate 
	float DamageAbsorbedByArmour = FMath::Min(BaseDamage - HealthDamageFromArPen, AttributeTotalValues.ArmourCurrentValue / (DamageType->ArmourDamage * 0.01f));

	OutHealthDamage = HealthDamageFromArPen + ((BaseDamage - HealthDamageFromArPen) - DamageAbsorbedByArmour);

	// Calculate armour damage
	OutArmourDamage = DamageAbsorbedByArmour * (DamageType->ArmourDamage * 0.01f);

	AddValueToAttributeBaseValues(EAttributeName::HealthCurrent, -OutHealthDamage);
	AddValueToAttributeBaseValues(EAttributeName::ArmourCurrent, -OutArmourDamage);
}


void USoSCombatComponent::AddValueToAttributeData(FAttributeData& AttributeData, EAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EAttributeName::HealthMax:
		AttributeData.HealthMaxValue += Value;
		break;
	case EAttributeName::HealthCurrent:
		AttributeData.HealthCurrentValue += Value;
		break;
	case EAttributeName::ArmourMax:
		AttributeData.ArmourMaxValue += Value;
		break;
	case EAttributeName::ArmourCurrent:
		AttributeData.ArmourCurrentValue += Value;
		break;
	case EAttributeName::EnergyMax:
		AttributeData.EnergyMaxValue += Value;
		break;
	case EAttributeName::EnergyCurrent:
		AttributeData.EnergyCurrentValue += Value;
		break;
	case EAttributeName::Speed:
		AttributeData.SpeedValue += Value;
		break;
	default:
		break;
	}
}


void USoSCombatComponent::MultiplyAttributeDataByValue(FAttributeData& AttributeData, EAttributeName Attribute, float Value)
{
	switch (Attribute)
	{
	case EAttributeName::HealthMax:
		AttributeData.HealthMaxValue *= Value;
		break;
	case EAttributeName::HealthCurrent:
		AttributeData.HealthCurrentValue *= Value;
		break;
	case EAttributeName::ArmourMax:
		AttributeData.ArmourMaxValue *= Value;
		break;
	case EAttributeName::ArmourCurrent:
		AttributeData.ArmourCurrentValue *= Value;
		break;
	case EAttributeName::EnergyMax:
		AttributeData.EnergyMaxValue *= Value;
		break;
	case EAttributeName::EnergyCurrent:
		AttributeData.EnergyCurrentValue *= Value;
		break;
	case EAttributeName::Speed:
		AttributeData.SpeedValue *= Value;
		break;
	default:
		break;
	}
}


void USoSCombatComponent::CalculateAttributeTotalValues()
{
	AttributeTotalValues.HealthMaxValue = FMath::Max(1.0f, AttributeBaseValues.HealthMaxValue * AttributeTempMultiplierValues.HealthMaxValue + AttributeTempAdditiveValues.HealthMaxValue);

	AttributeTotalValues.HealthCurrentValue = FMath::Clamp(AttributeBaseValues.HealthCurrentValue * AttributeTempMultiplierValues.HealthCurrentValue + AttributeTempAdditiveValues.HealthCurrentValue, 0.0f, AttributeTotalValues.HealthMaxValue);
	AttributeBaseValues.HealthCurrentValue = FMath::Clamp(AttributeBaseValues.HealthCurrentValue, 0.0f, AttributeTotalValues.HealthMaxValue + (AttributeBaseValues.HealthCurrentValue - (AttributeBaseValues.HealthCurrentValue * AttributeTempMultiplierValues.HealthCurrentValue + AttributeTempAdditiveValues.HealthCurrentValue)));

	AttributeTotalValues.ArmourMaxValue = FMath::Max(0.0f, AttributeBaseValues.ArmourMaxValue * AttributeTempMultiplierValues.ArmourMaxValue + AttributeTempAdditiveValues.ArmourMaxValue);

	AttributeTotalValues.ArmourCurrentValue = FMath::Clamp(AttributeBaseValues.ArmourCurrentValue * AttributeTempMultiplierValues.ArmourCurrentValue + AttributeTempAdditiveValues.ArmourCurrentValue, 0.0f, AttributeTotalValues.ArmourMaxValue);
	AttributeBaseValues.ArmourCurrentValue = FMath::Clamp(AttributeBaseValues.ArmourCurrentValue, 0.0f, AttributeTotalValues.ArmourMaxValue + (AttributeBaseValues.ArmourCurrentValue - (AttributeBaseValues.ArmourCurrentValue * AttributeTempMultiplierValues.ArmourCurrentValue + AttributeTempAdditiveValues.ArmourCurrentValue)));

	AttributeTotalValues.EnergyMaxValue = FMath::Max(0.0f, AttributeBaseValues.EnergyMaxValue * AttributeTempMultiplierValues.EnergyMaxValue + AttributeTempAdditiveValues.EnergyMaxValue);

	AttributeTotalValues.EnergyCurrentValue = FMath::Clamp(AttributeBaseValues.EnergyCurrentValue * AttributeTempMultiplierValues.EnergyCurrentValue + AttributeTempAdditiveValues.EnergyCurrentValue, 0.0f, AttributeTotalValues.EnergyMaxValue);
	AttributeBaseValues.EnergyCurrentValue = FMath::Clamp(AttributeBaseValues.EnergyCurrentValue, 0.0f, AttributeTotalValues.EnergyMaxValue + (AttributeBaseValues.EnergyCurrentValue - (AttributeBaseValues.EnergyCurrentValue * AttributeTempMultiplierValues.EnergyCurrentValue + AttributeTempAdditiveValues.EnergyCurrentValue)));

	AttributeTotalValues.SpeedValue = FMath::Max(0.0f, AttributeBaseValues.SpeedValue * AttributeTempMultiplierValues.SpeedValue + AttributeTempAdditiveValues.SpeedValue);

	if (OwningCharacter == nullptr)
	{
		return;
	}

	if (AttributeTotalValues.HealthCurrentValue == 0.0f)
	{
		OwnerState = EOwnerState::Dead;
	}

	if (UCharacterMovementComponent* CharacterMovement = OwningCharacter->GetCharacterMovement())
	{
		CharacterMovement->MaxWalkSpeed = AttributeTotalValues.SpeedValue;


		if (OwnerState == EOwnerState::Dead)
		{
			CharacterMovement->DisableMovement();
		}
	}
}


void USoSCombatComponent::AddValueToAttributeBaseValues(EAttributeName Attribute, float Value)
{
	AddValueToAttributeData(AttributeBaseValues, Attribute, Value);
}


void USoSCombatComponent::AddEffectToArray(FEffectData& EffectToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect Added: %s"), *EffectToAdd.EffectName.ToString())
	CurrentEffects.Add(EffectToAdd);
	for (EAbilityTag Tag : EffectToAdd.EffectAppliesTags)
	{
		CurrentEffectTags.Add(Tag);
		OnTagUpdate.Broadcast(Tag, ETagUpdateEventType::Added);
	}

	OnEffectUpdate.Broadcast(EffectToAdd, EEffectUpdateEventType::Added);
}


void USoSCombatComponent::RemoveEffectFromArrayByIndex(int32 Index)
{
	OnEffectUpdate.Broadcast(CurrentEffects[Index], EEffectUpdateEventType::Removed);

	UE_LOG(LogTemp, Warning, TEXT("Effect Removed: %s"), *CurrentEffects[Index].EffectName.ToString())
	CurrentEffects.RemoveAt(Index);
}


void USoSCombatComponent::RemoveEffectFromArrayByIndexArray(const TArray<int32>& EffectIndexesToRemove)
{
	for (int32 Index = EffectIndexesToRemove.Num()-1; Index >= 0; Index--)
	{
		RemoveEffectFromArrayByIndex(EffectIndexesToRemove[Index]);
	}
}


void USoSCombatComponent::EndEffect(FEffectData& EffectToEnd)
{
	if (EffectToEnd.bExpired)
	{
		return;
	}

	EffectToEnd.bExpired = true;

	for (FEffectAttributeModifierModule& Module : EffectToEnd.AttributeModifierModules)
	{
		if (Module.bTemporaryModifier)
		{
			Module.TotalValue = -Module.TotalValue;
			HandleEffectAttributeModifierValue(EffectToEnd, Module, true);
		}
	}

	for (FEffectOnCombatEventAbilityModule& Module : EffectToEnd.OnEventAbilityModules)
	{
		Module.OnCombatEventAbility->UnbindCombatEvent(EffectToEnd.SourceCombatComp, Module.AbilityTriggerType);
	}

	for (EAbilityTag Tag : EffectToEnd.EffectAppliesTags)
	{
		CurrentEffectTags.Remove(Tag);
		OnTagUpdate.Broadcast(Tag, ETagUpdateEventType::Removed);
	}
}


bool USoSCombatComponent::CheckAbilityCanCast(USoSAbilityBase* Ability, bool bTriggerCooldownAndResource)
{
	if (Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability is NULL"));
		return false;
	}

	if (OwnerState == EOwnerState::PerformingAction && !Ability->GetComboReady() && Ability->GetCastType() != EAbilityCastType::Instant)
	{
		return false;
	}

	if (!AbilityCheckCooldownAndCharges(Ability, bTriggerCooldownAndResource))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability %s on cooldown"), *Ability->GetName());
		return false;
	}

	if (AbilityCheckResource(Ability->GetResourceType(), Ability->GetCost(), bTriggerCooldownAndResource))
	{
		return true;
	}

	return false;
}


bool USoSCombatComponent::UseAbility(USoSAbilityBase* Ability, bool bReleased, float ClassSpecificFloatValue /*= 0*/)
{
	if (CheckAbilityCanCast(Ability, bReleased))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Cast: %s"), *Ability->GetName());

		UWorld* World = GetWorld();

		if (bReleased)
		{
			Ability->SetLastTimeActivated(World->GetTimeSeconds());
		}
		return bReleased == false ? Ability->StartAbility(this, ClassSpecificFloatValue) : Ability->ReleaseAbility(this, ClassSpecificFloatValue);
	}

	return false;
}


void USoSCombatComponent::AbilityActionStart()
{
	ASoSPlayerCharacter* Player = Cast<ASoSPlayerCharacter>(OwningCharacter);
	if (Player != nullptr)
	{
		Player->SprintEnd();
		Player->AimEnd();
	}

	OwnerState = EOwnerState::PerformingAction;
}


void USoSCombatComponent::AbilityActionComplete()
{
	LastAbilityToStartMontage->ActionComplete();

	OwnerState = EOwnerState::Normal;
}


bool USoSCombatComponent::AbilityCheckCooldownAndCharges(USoSAbilityBase* AbilityToCheck, bool bTriggerCooldown)
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

	if (bTriggerCooldown)
	{
		AbilityToCheck->SetLastChargeRemainder(NewCharges - FMath::TruncToInt(NewCharges));
		AbilityToCheck->SetCurrentCharges(AbilityToCheck->GetCurrentCharges() - 1);
	}
	return true;
}


bool USoSCombatComponent::AbilityCheckResource(EAbilityResourceType Type, float Cost, bool bUseResources)
{
	switch (Type)
	{
	case EAbilityResourceType::Energy:
		if (AttributeTotalValues.EnergyCurrentValue < Cost)
		{
			return false;
		}

		if (bUseResources)
		{
			AddValueToAttributeData(AttributeBaseValues, EAttributeName::EnergyCurrent, -Cost);
		}
		break;
	case EAbilityResourceType::Health:
		if (AttributeTotalValues.HealthCurrentValue+1 < Cost)
		{
			return false;
		}

		if (bUseResources)
		{
			AddValueToAttributeData(AttributeBaseValues, EAttributeName::HealthCurrent, -Cost);
		}
		break;
	default:
		break;
	}

	

	return true;
}


////////////////////////////////////////////////
// Getters and Setters


float USoSCombatComponent::GetAttributeDataValue(FAttributeData* AttributeData, EAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EAttributeName::HealthMax:
		return AttributeData->HealthMaxValue;
		break;
	case EAttributeName::HealthCurrent:
		return AttributeData->HealthCurrentValue;
		break;
	case EAttributeName::ArmourMax:
		return AttributeData->ArmourMaxValue;
		break;
	case EAttributeName::ArmourCurrent:
		return AttributeData->ArmourCurrentValue;
		break;
	case EAttributeName::EnergyMax:
		return AttributeData->EnergyMaxValue;
		break;
	case EAttributeName::EnergyCurrent:
		return AttributeData->EnergyCurrentValue;
		break;
	case EAttributeName::Speed:
		return AttributeData->SpeedValue;
		break;
	default:
		return -1;
		break;
	}
}


float USoSCombatComponent::GetAttributeTotalValue(EAttributeName AttributeToGet) const
{
	switch (AttributeToGet)
	{
	case EAttributeName::HealthMax:
		return AttributeTotalValues.HealthMaxValue;
		break;
	case EAttributeName::HealthCurrent:
		return AttributeTotalValues.HealthCurrentValue;
		break;
	case EAttributeName::ArmourMax:
		return AttributeTotalValues.ArmourMaxValue;
		break;
	case EAttributeName::ArmourCurrent:
		return AttributeTotalValues.ArmourCurrentValue;
		break;
	case EAttributeName::EnergyMax:
		return AttributeTotalValues.EnergyMaxValue;
		break;
	case EAttributeName::EnergyCurrent:
		return AttributeTotalValues.EnergyCurrentValue;
		break;
	case EAttributeName::Speed:
		return AttributeTotalValues.SpeedValue;
		break;
	default:
		return -1;
		break;
	}
}


TArray<FEffectData>& USoSCombatComponent::GetCurrentEffectsArray()
{
	return CurrentEffects;
}


TArray<EAbilityTag>& USoSCombatComponent::GetCurrentEffectTags()
{
	return CurrentEffectTags;
}


EOwnerState USoSCombatComponent::GetOwnerState() const
{
	return OwnerState;
}


USoSAbilityBase* USoSCombatComponent::GetLastAbilityToStartMontage() const
{
	return LastAbilityToStartMontage;
}


ASoSCharacterBase* USoSCombatComponent::GetOwningCharacter() const
{
	return OwningCharacter;
}

ESoSTeam USoSCombatComponent::GetTeam() const
{
	return Team;
}


void USoSCombatComponent::SetOwnerState(EOwnerState NewState)
{
	OwnerState = NewState;
}


void USoSCombatComponent::SetWeaponProjectileOriginSocketName(FName SocketName)
{
	WeaponProjectileOriginSocketName = SocketName;
}


void USoSCombatComponent::SetLastAbilityToStartMontage(USoSAbilityBase* Ability)
{
	LastAbilityToStartMontage = Ability;
}

