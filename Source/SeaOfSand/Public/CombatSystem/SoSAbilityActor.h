// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoSAbilityActor.generated.h"

UCLASS()
class SEAOFSAND_API ASoSAbilityActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSAbilityActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

protected:

	UPROPERTY(BlueprintReadOnly, Category = "AbilityActor")
	AActor* AbilityActorSource;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityActor")
	float MaxLifetime;

	UFUNCTION(BlueprintNativeEvent, Category = "AbilityActor")
	void EndAbilityActor();
	virtual void EndAbilityActor_Implementation();
	
	FTimerHandle TimerHandle_AreaEffectTick;

public:

	AActor * GetAbilityActorSource() const;

	void SetAbilityActorSource(AActor* Source);
};
