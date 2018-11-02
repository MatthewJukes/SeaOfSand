// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSAbilityActor.h"
#include "Public/TimerManager.h"


// Sets default values
ASoSAbilityActor::ASoSAbilityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MaxLifetime = 999;
}

// Called when the game starts or when spawned
void ASoSAbilityActor::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_AreaEffectTick, this, &ASoSAbilityActor::EndAbilityActor, MaxLifetime, true);
}

void ASoSAbilityActor::EndAbilityActor_Implementation()
{
	Destroy();
}

