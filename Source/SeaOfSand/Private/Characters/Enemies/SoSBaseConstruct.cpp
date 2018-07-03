// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseConstruct.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASoSBaseConstruct::ASoSBaseConstruct()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	//MovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("MovementComp"));
}


void ASoSBaseConstruct::BeginPlay()
{
	Super::BeginPlay();

}

