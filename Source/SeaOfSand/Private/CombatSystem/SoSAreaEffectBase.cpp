// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSAreaEffectBase.h"
#include "Public/TimerManager.h"


// Sets default values
ASoSAreaEffectBase::ASoSAreaEffectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MaxLifetime = 2;
	TickRate = 0.25;
}

// Called when the game starts or when spawned
void ASoSAreaEffectBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_AreaEffectTick, this, &ASoSAreaEffectBase::AreaEffectTick, TickRate, true);
}

void ASoSAreaEffectBase::EndAbilityActor_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_AreaEffectTick);

	Destroy();
}

