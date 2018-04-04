// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVehicle.h"
#include "HoverComponent.h"
#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"


// Sets default values
ABaseVehicle::ABaseVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
	RootComponent = VehicleMesh;

	// Setup hover components
	HoverComponent1 = CreateDefaultSubobject<UHoverComponent>(TEXT("HoverComponent1"));
	HoverComponent2 = CreateDefaultSubobject<UHoverComponent>(TEXT("HoverComponent2"));
	HoverComponent3 = CreateDefaultSubobject<UHoverComponent>(TEXT("HoverComponent3"));
	HoverComponent4 = CreateDefaultSubobject<UHoverComponent>(TEXT("HoverComponent4"));
	HoverComponent1->SetupAttachment(RootComponent);
	HoverComponent2->SetupAttachment(RootComponent);
	HoverComponent3->SetupAttachment(RootComponent);
	HoverComponent4->SetupAttachment(RootComponent);

	//Test Arrows
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	ForwardArrow->SetupAttachment(RootComponent);
	UpArrow->SetupAttachment(RootComponent);
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
	ForwardArrow->SetWorldRotation(CalculateGroundForwardVector().Rotation());
	UpArrow->SetWorldRotation(CalculateGroundUpVector().Rotation());
}

// Called to bind functionality to input
void ABaseVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseVehicle::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseVehicle::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseVehicle::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseVehicle::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABaseVehicle::BoostStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABaseVehicle::BoostEnd);
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

void ABaseVehicle::MoveForward(float AxisValue)
{
	FVector Force = CalculateGroundForwardVector() * ForwardThrust * AxisValue;
	VehicleMesh->AddForce(Force);
}

void ABaseVehicle::MoveRight(float AxisValue)
{
	// Calculate if vehicle is moving forwards or backwards
	float TorqueMultiplier = FVector::DotProduct(VehicleMesh->GetPhysicsLinearVelocity(), VehicleMesh->GetForwardVector());
	if (TorqueMultiplier >= 0) // If moving forward set multiplier to 1
	{
		TorqueMultiplier = 1;
	}
	else // If moving backwards invert torque multiplier
	{
		TorqueMultiplier = -1;
	}
	float TorqueStrength = AxisValue * TurningThrust;
	FVector RotationTorque = CalculateGroundUpVector() * TorqueStrength * TorqueMultiplier;
	VehicleMesh->AddTorque(RotationTorque);
}

void ABaseVehicle::BoostStart()
{
	ForwardThrust *= BoostMultiplier;
}

void ABaseVehicle::BoostEnd()
{
	ForwardThrust /= BoostMultiplier;
}

FVector ABaseVehicle::CalculateGroundForwardVector()
{
	FVector Forward = ((HoverComponent1->HitLocation + HoverComponent2->HitLocation) - (HoverComponent3->HitLocation + HoverComponent4->HitLocation));
	Forward.Normalize();	
	return Forward;
}

FVector ABaseVehicle::CalculateGroundUpVector()
{
	FVector Up = (HoverComponent1->HitNormal + HoverComponent2->HitNormal + HoverComponent3->HitNormal + HoverComponent4->HitNormal) / 4;
	return Up;
}

