// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVehicle.h"
#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"


// Sets default values
ABaseVehicle::ABaseVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void ABaseVehicle::BeginPlay()
{
	Super::BeginPlay();	
	
}

// Called every frame
void ABaseVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseVehicle::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseVehicle::AddControllerPitchInput);
}

bool ABaseVehicle::Interact_Implementation()
{
	// Respawn player character
	FActorSpawnParameters SpawnParams;
	FVector SpawnLocation = GetActorLocation() + FVector(0.f,300.f,0.f);
	FRotator SpawnRotation = GetActorRotation();
	APlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterBlueprint, SpawnLocation, SpawnRotation, SpawnParams);
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetController());

	// Possess player pawn
	if (PlayerCharacter && PlayerController)
	{
		PlayerCharacter->GetWorld();
		PlayerController->Possess(PlayerCharacter);
		PlayerController->UpdateCurrentPawn();
	}
	return false;
}

