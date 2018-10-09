// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSWeaponBase.h"
#include "SoSMeleeWeapon.generated.h"

class UCapsuleComponent;

UCLASS()
class SEAOFSAND_API ASoSMeleeWeapon : public ASoSWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSMeleeWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCapsuleComponent* DamageCapsule;

public:	

	void StartAttack() override;

	void EndAttack() override;
	
	UFUNCTION(BlueprintCallable, Category = "MeleeWeapon")
	UCapsuleComponent* GetDamageCapsule() const;
};
