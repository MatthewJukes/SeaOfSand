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
#include "Public/TimerManager.h"

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

	// Socket names
	DriverAttachPoint = TEXT("DriverSeatSocket");
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

	CurrentForwardThrust = BaseForwardThrust;
	CurrentTurningThrust = BaseTurningThrust;
	CurrentHeat = HeatThresholds[0];
	Health = MaxHealth;

	GetWorldTimerManager().SetTimer(HeatTimerHandle, this, &ABaseVehicle::CheckHeatLevels, 0.1f, true, 0.1f);
}

// Called every frame
void ABaseVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetDamping();
	RollCorrection();
	PitchCorrection();

	Speed = this->GetVelocity().Size()*0.036f;	
}

// Exit vehicle
bool ABaseVehicle::Interact_Implementation()
{
	FTransform SpawnTransform = GetTransform();
	SpawnTransform.AddToTranslation((VehicleMesh->GetRightVector() * 200.f) + FVector(0.f,0.f,50.f)); // TODO refine spawning position
	FVector RelativeForward = VehicleMesh->GetForwardVector();
	RelativeForward.Z = 0.f;
	RelativeForward.Normalize();
	SpawnTransform.SetRotation(FQuat(RelativeForward.Rotation()));	

	// Possess player pawn
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetController());
	if (PlayerController && CurrentDriver)
	{
		if (bIsBoosting) { BoostEnd(); }
		CurrentDriver->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		CurrentDriver->AddActorLocalTransform(SpawnTransform);
		CurrentDriver->EnableCollsion();
		CurrentDriver->bInVehicle = false;
		PlayerController->Possess(CurrentDriver);
		PlayerController->UpdateCurrentPawn();
		PlayerController->ToggleVehicleHud();
		return true;
	}
	return false;
}

void ABaseVehicle::MoveForward(float AxisValue)
{
	AxisValue = FMath::Clamp(AxisValue, -0.5f, 1.f);
	FVector Force = GetGroundForwardVector() * CurrentForwardThrust * AxisValue * GetTractionRatio();
	VehicleMesh->AddForce(Force);
}

void ABaseVehicle::MoveRight(float AxisValue)
{
	float TorqueStrength = AxisValue * CurrentTurningThrust;
	FVector Torque = GetGroundUpVector() * TorqueStrength * GetTractionRatio(true);
	VehicleMesh->AddTorqueInRadians(Torque);
}

void ABaseVehicle::BoostStart() // TODO increase strength overtime and impliment overheating system
{
	if (!bIsBoosting)
	{
		bIsBoosting = true;		
		GetWorldTimerManager().SetTimer(BoostTimerHandle, this, &ABaseVehicle::Boost, 0.1f, true, 0.1f);
	}
}

void ABaseVehicle::BoostEnd()
{
	if (bIsBoosting)
	{
		bIsBoosting = false;
		GetWorldTimerManager().ClearTimer(BoostTimerHandle);
		CurrentBoost = 1.f;
		CurrentForwardThrust = BaseForwardThrust;
		CurrentTurningThrust = BaseTurningThrust;
	}
}

void ABaseVehicle::Boost()
{ 
	FVector2D InputRange = FVector2D(HeatThresholds[1], HeatThresholds[2]);
	CurrentBoost = FMath::GetMappedRangeValueClamped(InputRange, BoostMultiplierRange, CurrentHeat);
	CurrentForwardThrust = BaseForwardThrust * CurrentBoost;
	CurrentTurningThrust = BaseTurningThrust / CurrentBoost;

	//UE_LOG(LogTemp, Warning, TEXT("Current Boost: %f"), CurrentBoost);
}

void ABaseVehicle::CheckHeatLevels()
{
	//UE_LOG(LogTemp, Warning, TEXT("Heat Level: %f"), CurrentHeat);

	if (CurrentHeat > HeatThresholds[3] && bIsBoosting)
	{		
		FVector2D InputRange = FVector2D(HeatThresholds[3], HeatThresholds[4]);
		FVector2D OutputRange = FVector2D(0.f, 1.f);
		float CurvePosition = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, CurrentHeat);
		float DamageRate = DamageRateCurve->GetFloatValue(CurvePosition);
		Health -= DamageRate;
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	}

	SetHeat();
}

void ABaseVehicle::SetHeat()
{
	if (Speed < 3) // Vehicle at idle
	{
		CurrentHeat -= CoolingRateRange.X * 0.1f; // cool by first rate
		CurrentHeat = FMath::Clamp(CurrentHeat, HeatThresholds[0], HeatThresholds[4]); // clamp to idle min temperature
	}
	else if (!bIsBoosting) // Not currently boosting
	{
		if (CurrentHeat <= HeatThresholds[1]) // and heat below normal threshold
		{
			CurrentHeat += HeatingRateRange.X * 0.1f; // increase heat at first rate
		}
		else if (CurrentHeat <= HeatThresholds[4]) // Heat is above normal threshold but below soft cap
		{
			CurrentHeat -= CoolingRateRange.Y * 0.1f; // Cool at second rate
		}
		else
		{
			CurrentHeat -= CoolingRateRange.Z * 0.1f; // Cool at third rate
		}
	}
	else // Currently boosting
	{
		if (CurrentHeat <= HeatThresholds[1]) // and heat still below normal threshold
		{
			CurrentHeat += (HeatingRateRange.X + HeatingRateRange.Y) * 0.1f; // increase heat at combined rate
		}
		else if (CurrentHeat <= HeatThresholds[4]) // Heat is above normal threshold but below soft cap
		{
			CurrentHeat += HeatingRateRange.Y * 0.1f; // increase heat at second rate
		}
		else
		{
			CurrentHeat += HeatingRateRange.Z * 0.1f; // heat at third rate
		}
	}

}

FVector ABaseVehicle::GetGroundForwardVector() const
{
	FVector Forward = ((HoverComponent1->HitLocation + HoverComponent2->HitLocation) - (HoverComponent3->HitLocation + HoverComponent4->HitLocation));
	Forward.Normalize();	
	return Forward;
}

FVector ABaseVehicle::GetGroundUpVector() const
{
	FVector Up = (HoverComponent1->HitNormal + HoverComponent2->HitNormal + HoverComponent3->HitNormal + HoverComponent4->HitNormal) / 4;
	return Up;
}

float ABaseVehicle::GetGroundIncline() const
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
	float ForceRatio = FVector::DotProduct(VehicleMesh->GetRightVector(), GoalVector) * GetTractionRatio();
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

float ABaseVehicle::GetTotalShortCompressionRatio() const
{
	return HoverComponent1->ShortCompressionRatio + HoverComponent2->ShortCompressionRatio + HoverComponent3->ShortCompressionRatio + HoverComponent4->ShortCompressionRatio;
}

float ABaseVehicle::GetTotalLongCompressionRatio() const
{
	return HoverComponent1->LongCompressionRatio + HoverComponent2->LongCompressionRatio + HoverComponent3->LongCompressionRatio + HoverComponent4->LongCompressionRatio;
}

float ABaseVehicle::GetTractionRatio(const bool bUseLongCompressionRatio) const // TODO test physics materials for unclimbable barriers
{
	FVector2D InputRange = FVector2D(3.5f, 4.f);
	FVector2D OutputRange = FVector2D(1.f, 0.1f);
	if (bUseLongCompressionRatio)
	{
		return FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetTotalLongCompressionRatio());
	}
	else 
	{
		FVector2D VelocityInputRange = FVector2D(50.f, 200.f); // In km/h
		FVector2D VelocityOutputRange = FVector2D(0.f, .65f);

		float SpeedTractionBoost = FMath::GetMappedRangeValueClamped(VelocityInputRange, VelocityOutputRange, this->GetVelocity().Size()*0.036f);
		//UE_LOG(LogTemp, Warning, TEXT("%f traction boost"), SpeedTractionBoost);

		float Traction = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetTotalShortCompressionRatio());
		Traction = FMath::Lerp(Traction, 0.01f, FMath::Lerp(GetGroundIncline(), 0.f, SpeedTractionBoost)); // Reduce traction if on steep slope, boost traction again if going fast
		//UE_LOG(LogTemp, Warning, TEXT("%f traction"), Traction);

		return Traction;
	}
}

