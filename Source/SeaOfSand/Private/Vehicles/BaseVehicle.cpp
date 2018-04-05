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

// Called when the game starts or when spawned
void ABaseVehicle::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABaseVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetDamping();
	RollTowardsGroundPlaneNormal();

	ForwardArrow->SetWorldRotation(GetGroundForwardVector().Rotation());
	UpArrow->SetWorldRotation(GetGroundUpVector().Rotation());

	//UE_LOG(LogTemp, Warning, TEXT("%f km/h"), this->GetVelocity().Size()*0.036f)
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
	AxisValue = FMath::Clamp(AxisValue, -0.25f, 1.f);
	FVector Force = GetGroundForwardVector() * ForwardThrust * AxisValue * GetTractionRatio();
	VehicleMesh->AddForce(Force);
}

void ABaseVehicle::MoveRight(float AxisValue)
{
	float TorqueStrength = AxisValue * TurningThrust;
	FVector Torque = GetGroundUpVector() * TorqueStrength;
	VehicleMesh->AddTorque(Torque);
}

void ABaseVehicle::BoostStart()
{
	ForwardThrust *= BoostMultiplier;
}

void ABaseVehicle::BoostEnd()
{
	ForwardThrust /= BoostMultiplier;
}

FVector ABaseVehicle::GetGroundForwardVector()
{
	FVector Forward = ((HoverComponent1->HitLocation + HoverComponent2->HitLocation) - (HoverComponent3->HitLocation + HoverComponent4->HitLocation));
	Forward.Normalize();	
	return Forward;
}

FVector ABaseVehicle::GetGroundUpVector()
{
	FVector Up = (HoverComponent1->HitNormal + HoverComponent2->HitNormal + HoverComponent3->HitNormal + HoverComponent4->HitNormal) / 4;
	return Up;
}

void ABaseVehicle::SetDamping()
{
	float Traction = GetTractionRatio();
	VehicleMesh->SetLinearDamping(FMath::Lerp(0.f, 2.f, Traction));
}

void ABaseVehicle::RollTowardsGroundPlaneNormal()
{
	float ForceRatio = FVector::DotProduct(VehicleMesh->GetRightVector(), FVector(0.f,0.f,1.f));
	UE_LOG(LogTemp, Warning, TEXT("Dot product: %f"), ForceRatio);
	FVector Torque = VehicleMesh->GetForwardVector() * -ForceRatio * RollOrientStrength;
	VehicleMesh->AddTorque(Torque);
}

float ABaseVehicle::GetTotalCompressionRatio()
{
	return HoverComponent1->CompressionRatio + HoverComponent2->CompressionRatio + HoverComponent3->CompressionRatio + HoverComponent4->CompressionRatio;
}

float ABaseVehicle::GetTractionRatio()
{
	FVector2D InputRange = FVector2D(3.5f, 4.f);
	FVector2D OutputRange = FVector2D(1.f, 0.1f);
	return FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetTotalCompressionRatio());
}

