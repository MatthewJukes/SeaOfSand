// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVehicle.h"
#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


// Sets default values
ABaseVehicle::ABaseVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
	RootComponent = VehicleMesh;
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

// Exit vehicle
bool ABaseVehicle::Interact_Implementation()
{
	// Respawn player character
	FActorSpawnParameters SpawnParams; 
	FTransform SpawnTransform = GetTransform(); // TODO refine spawning position
	SpawnTransform.AddToTranslation(FVector(0.f,300.f,100.f));
	SpawnTransform.SetRotation(FQuat(0.f,0.f,0.f,0.f));
	APlayerCharacter* PlayerCharacter = GetWorld()->SpawnActorDeferred<APlayerCharacter>(PlayerCharacterBP, SpawnTransform);
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetController());

	// Possess player pawn
	if (PlayerCharacter && PlayerController)
	{
		PlayerController->Possess(PlayerCharacter);
		PlayerController->UpdateCurrentPawn();
		PlayerCharacter->FinishSpawning(SpawnTransform);		
		if (!IsValid(PlayerCharacter)) // Reposses vehicle is spawn failed
		{
			PlayerController->Possess(this);
			PlayerController->UpdateCurrentPawn();
		}
	}
	return false;
}

