// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSHealthComponent.h"


USoSHealthComponent::USoSHealthComponent()
{
	BaseHealth = 100;
	bIsDead = false;

	TeamNum = 255;
}

void USoSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USoSHealthComponent::HandleTakeAnyDamage);
	}
	
	CurrentHealth = BaseHealth;
}

void USoSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	// Update health clamped
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, BaseHealth);

	UE_LOG(LogTemp, Log, TEXT("%s Health Changed: %s (-%s), Damaged by %s"), *DamagedActor->GetName(), *FString::SanitizeFloat(CurrentHealth), *FString::SanitizeFloat(Damage), *DamageCauser->GetName());

	bIsDead = CurrentHealth <= 0.0f;

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{ /*
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		} */
	}
}

void USoSHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, BaseHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(CurrentHealth), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, CurrentHealth, -HealAmount, nullptr, nullptr, nullptr);
}

bool USoSHealthComponent::IsFriendly(AActor * ActorA, AActor * ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	USoSHealthComponent* HealthCompA = Cast<USoSHealthComponent>(ActorA->GetComponentByClass(USoSHealthComponent::StaticClass()));
	USoSHealthComponent* HealthCompB = Cast<USoSHealthComponent>(ActorB->GetComponentByClass(USoSHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

/////////////////////////
/* Getters and Setters */
/////////////////////////

float USoSHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float USoSHealthComponent::GetBaseHealth() const
{
	return BaseHealth;
}
