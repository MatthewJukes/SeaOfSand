// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "BasePlayerController.h"
#include "BaseWeapon.h"
#include "BaseVehicle.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


// Set defaults
APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Setup camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 40.f);

	// Setup camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Socket names
	WeaponAttachPoint = TEXT("WeaponSocket");
	BackHolsterAttachPoint = TEXT("BackHolsterSocket");
}

// Bind Inputs specific to player on foot
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::SprintEnd);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::CrouchEnd);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::AimStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::AimEnd);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::JumpAndFlip);	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	
	PlayerController = Cast<ABasePlayerController>(GetController());

	SpawnWeapon();
	HolsterUnholster();	
}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		// Find out which was is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		// Find out which was is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::SprintStart()
{
	if (GetVelocity().Size() > 0.01)
	{
		if (bIsAiming) { AimEnd(); }
		if (bWeaponIsDrawn) { HolsterUnholster(); }
		bIsSprinting = true;
		SprintZoom(true); // Call BP timeline, playing forward
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SprintMultiplier;
	}
}

void APlayerCharacter::SprintEnd()
{
	if (bIsSprinting) // Only execute if sprinting
	{
		bIsSprinting = false;
		SprintZoom(false); // Call BP timeline, playing backwards
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	}
}

void APlayerCharacter::CrouchStart()
{
	// TODO add player crouching
}

void APlayerCharacter::CrouchEnd()
{
}

void APlayerCharacter::AimStart()
{
	if (bIsSprinting) { SprintEnd(); }
	if (!bWeaponIsDrawn) { HolsterUnholster(); }
	bIsAiming = true;
	AimZoom(true); // Call BP timeline, playing forward
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * AimMultiplier;

	if (CurrentWeapon) // Give weapon bonus accuracy
	{
		CurrentWeapon->bAimingBonus = true;
	}
}

void APlayerCharacter::AimEnd()
{
	if (bIsAiming) // Only execute if aiming
	{
		bIsAiming = false;
		AimZoom(false); // Call BP timeline, playing backwards

		if (bWeaponIsDrawn)
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * WeaponDrawnMultiplier;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		}

		if (CurrentWeapon) // Remove weapon bonus accuracy
		{
			CurrentWeapon->bAimingBonus = false;
		}
	}
}

void APlayerCharacter::JumpAndFlip()
{
	// TODO add double jump or flip and play test
	Jump();
}

void APlayerCharacter::SpawnWeapon()
{
	FActorSpawnParameters SpawnParams;
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(RifleBlueprint, SpawnParams);

	if (PlayerController) {	PlayerController->UpdateCurrentWeapon(CurrentWeapon); }
}

void APlayerCharacter::HolsterUnholster()
{
	if (CurrentWeapon)
	{
		if (bWeaponIsDrawn) // Holster weapon
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackHolsterAttachPoint);
			bCanFire = false;
			bWeaponIsDrawn = false;
			OffsetCamera(false);

			// Update character controller settings
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}
		else // Unholster weapon
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachPoint);
			bCanFire = true;
			bWeaponIsDrawn = true;
			OffsetCamera(true);

			// Update character controller settings
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * WeaponDrawnMultiplier;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
}

void APlayerCharacter::EnableCollsion()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool APlayerCharacter::Interact_Implementation()
{
	AActor* ActorHit = nullptr;
	if (InteractTrace(ActorHit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *GetNameSafe(ActorHit));

		ABaseVehicle* Vehicle = Cast<ABaseVehicle>(ActorHit);
		if (Vehicle) // If a vehicle, possess this vehicle pawn
		{
			Vehicle->CurrentDriver = this;
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AttachToComponent(Vehicle->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Vehicle->DriverAttachPoint);

			bInVehicle = true;
			PlayerController->Possess(Vehicle);
			PlayerController->UpdateCurrentPawn();
			PlayerController->ToggleVehicleHud();
		}	
	}
	return false;
}

bool APlayerCharacter::InteractTrace(AActor* &OutActor) const
{
	const FName TraceTag("InteractTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;
	FVector StartLocation = GetActorLocation() + FVector(0.f,0.f,40.f);
	FVector EndLocation = StartLocation + (GetTraceDirection(StartLocation) * InteractTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{		
		OutActor = RV_Hit.GetActor();
		return true;
	}
	return false; // Trace didn't hit anything
}

FVector APlayerCharacter::GetTraceDirection(FVector StartLocation) const
{
	if (PlayerController)
	{
		FVector TraceDirection = PlayerController->GetCrosshairHitLocation() - StartLocation;
		TraceDirection.Normalize();
		return TraceDirection;
	}
	return FVector(0.f,0.f,0.f);
}
