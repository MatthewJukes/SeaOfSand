// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVehicle.h"
#include "HoverComponent.h"
#include "BasePlayerController.h"
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

	// Setup driver mesh
	DriverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DriverMesh"));
	DriverMesh->SetupAttachment(RootComponent);
	DriverMesh->SetVisibility(false);

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
	RollCorrection();
	PitchCorrection();

	ForwardArrow->SetWorldRotation(GetGroundForwardVector().Rotation());
	UpArrow->SetWorldRotation(GetGroundUpVector().Rotation());	

	UE_LOG(LogTemp, Warning, TEXT("%f km/h"), this->GetVelocity().Size()*0.036f);	
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
		if (bIsBoosting) { BoostEnd(); }
		PlayerController->Possess(PlayerCharacter);
		PlayerController->UpdateCurrentPawn();
		PlayerCharacter->FinishSpawning(SpawnTransform);
		ToggleDriverVisibility();
		if (!IsValid(PlayerCharacter)) // Reposses vehicle is spawn failed
		{
			PlayerController->Possess(this);
			PlayerController->UpdateCurrentPawn();
			ToggleDriverVisibility();
		}
	}
	return false;
}

void ABaseVehicle::ToggleDriverVisibility()
{
	DriverMesh->ToggleVisibility();
}

void ABaseVehicle::MoveForward(float AxisValue)
{
	AxisValue = FMath::Clamp(AxisValue, -0.5f, 1.f);
	FVector Force = GetGroundForwardVector() * ForwardThrust * AxisValue * GetTractionRatio();
	VehicleMesh->AddForce(Force);
}

void ABaseVehicle::MoveRight(float AxisValue)
{
	float TorqueStrength = AxisValue * TurningThrust;
	FVector Torque = GetGroundUpVector() * TorqueStrength * GetTractionRatio(true);
	VehicleMesh->AddTorqueInRadians(Torque);
}

void ABaseVehicle::BoostStart() // TODO increase strength overtime and impliment overheating system
{
	if (!bIsBoosting)
	{
		bIsBoosting = true;
		ForwardThrust *= BoostMultiplier;
		TurningThrust *= 0.5f;
	}
}

void ABaseVehicle::BoostEnd()
{
	if (bIsBoosting)
	{
		bIsBoosting = false;
		ForwardThrust /= BoostMultiplier;
		TurningThrust *= 2.f;
	}
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

float ABaseVehicle::GetGroundIncline()
{
	if (InclineTractionCurve)
	{
		float Incline = InclineTractionCurve->GetFloatValue(FVector::DotProduct(GetGroundUpVector(), FVector(0.f, 0.f, 1.f)));

		return 1.f - Incline;
	}
	
	return 0.f;
}

void ABaseVehicle::SetDamping()
{
	VehicleMesh->SetLinearDamping(FMath::Lerp(0.f, 3.f, GetTractionRatio()));
}

void ABaseVehicle::RollCorrection()
{
	FVector GoalVector = FMath::Lerp(GetGroundUpVector(), FVector(0.f, 0.f, 1.f), GetTractionRatio(true));
	float ForceRatio = FVector::DotProduct(VehicleMesh->GetRightVector(), GoalVector);
	FVector Torque = VehicleMesh->GetForwardVector() * -ForceRatio * RollOrientStrength;
	VehicleMesh->AddTorqueInRadians(Torque);
}

void ABaseVehicle::PitchCorrection()
{
	FVector InAirVector = VehicleMesh->GetForwardVector();
	InAirVector.Z = -0.2f;
	InAirVector.Normalize();

	FVector GoalVector = FMath::Lerp(GetGroundForwardVector(), InAirVector, GetTractionRatio(true));
	float ForceRatio = FVector::DotProduct(VehicleMesh->GetUpVector(), GoalVector);

	FVector Torque = VehicleMesh->GetRightVector() * -ForceRatio * PitchOrientStrength;
	VehicleMesh->AddTorqueInRadians(Torque);
}

float ABaseVehicle::GetTotalShortCompressionRatio()
{
	return HoverComponent1->ShortCompressionRatio + HoverComponent2->ShortCompressionRatio + HoverComponent3->ShortCompressionRatio + HoverComponent4->ShortCompressionRatio;
}

float ABaseVehicle::GetTotalLongCompressionRatio()
{
	return HoverComponent1->LongCompressionRatio + HoverComponent2->LongCompressionRatio + HoverComponent3->LongCompressionRatio + HoverComponent4->LongCompressionRatio;
}

float ABaseVehicle::GetTractionRatio(bool bUseLongCompressionRatio)
{
	FVector2D InputRange = FVector2D(3.5f, 4.f);
	FVector2D OutputRange = FVector2D(1.f, 0.1f);
	if (bUseLongCompressionRatio)
	{
		return FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetTotalLongCompressionRatio());
	}
	else
	{
		FVector2D VelocityInputRange = FVector2D(30.f, 200.f); // In km/h
		FVector2D VelocityOutputRange = FVector2D(0.f, .8f);

		float SpeedTractionBoost = FMath::GetMappedRangeValueClamped(VelocityInputRange, VelocityOutputRange, this->GetVelocity().Size()*0.036f);
		UE_LOG(LogTemp, Warning, TEXT("%f traction boost"), SpeedTractionBoost);

		float Traction = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetTotalShortCompressionRatio());
		Traction = FMath::Lerp(Traction, 0.01f, FMath::Lerp(GetGroundIncline(), 0.f, SpeedTractionBoost)); // Reduce traction if on steep slope, boost traction again if going fast
		UE_LOG(LogTemp, Warning, TEXT("%f traction"), Traction);

		return Traction;
	}
}

