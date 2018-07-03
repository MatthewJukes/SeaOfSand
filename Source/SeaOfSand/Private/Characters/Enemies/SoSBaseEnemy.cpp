// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseEnemy.h"
#include "SoSHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"


ASoSBaseEnemy::ASoSBaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	EnemyHealth = CreateDefaultSubobject<USoSHealthComponent>(TEXT("EnemyHealth"));
}

void ASoSBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASoSBaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

