// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "PlayerInventory.h"
#include "BasePlayerController.h"
#include "BaseVehicle.h"
#include "BaseWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/TimerManager.h"


// Set defaults
APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Setup inventory
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));

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

	// Configure player vitals
	MaxHealth = 250.f;
	MaxStamina = 100.f;
	BaseStaminaRegenRate = 6.f;
	SprintStaminaDrainRate = 10.f;
	RollStaminaCost = 20.f;

	// Configure character movement
	BaseSpeed = 400.f;
	SprintMultiplier = 1.6f;
	AimMultiplier = 0.6f;
	WeaponDrawnMultiplier = 0.8f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	GetCharacterMovement()->AirControl = 0.2f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;	

	// Configure interact
	InteractTraceRange = 250.f;

	// Set bools
	bCanFire = false;
	bIsAiming = false;
	bWeaponIsDrawn = false;
	bIsRolling = false;
	bIsSprinting = false;
	bIsDoubleJumping = false;
	bCanDoubleJump = true;
	bInVehicle = false;
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::StartRoll);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Get controller
	PlayerController = Cast<ABasePlayerController>(GetController());

	// Set default health and stamina values
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	SetStaminaRate(BaseStaminaRegenRate);
}

void APlayerCharacter::SetStaminaRate(float RatePerSecond)
{
	float TickRate = 0.25f;
	
	// Set timers
	FTimerDelegate StaminaTimerDel;
	StaminaTimerDel.BindUFunction(this, FName("IncrementStamina"), RatePerSecond * TickRate);
	GetWorldTimerManager().SetTimer(StaminaTimerHandle, StaminaTimerDel, TickRate, true);
}

void APlayerCharacter::IncrementStamina(float Amount)
{
	CurrentStamina += Amount;
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);

	if (CurrentStamina <= 0.1f)
	{
		if (bIsSprinting) { SprintEnd(); }
	}
}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f) && !bIsRolling)
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
	if ((Controller != NULL) && (AxisValue != 0.0f) && !bIsRolling)
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
	if (GetVelocity().Size() > 0.01 && !GetCharacterMovement()->IsFalling() && !bIsRolling && CurrentStamina >= 5.f)
	{
		if (bIsAiming) { AimEnd(); }
		if (PlayerInventory->CurrentWeapon) { PlayerInventory->CurrentWeapon->InterruptReload(); }

		bIsSprinting = true;
		SprintZoom(true); // Call BP timeline, playing forward
		SetStaminaRate(-SprintStaminaDrainRate);

		if (bWeaponIsDrawn)
		{
			SetPlayerSpeed(WeaponDrawnMultiplier * SprintMultiplier);
			SetPlayerMovementType(true, false);
		}
		else
		{
			SetPlayerSpeed(SprintMultiplier);
		}
	}
}


void APlayerCharacter::SprintEnd()
{
	if (bIsSprinting) // Only execute if sprinting
	{
		bIsSprinting = false;
		SprintZoom(false); // Call BP timeline, playing backwards
		SetStaminaRate(BaseStaminaRegenRate);

		if (bWeaponIsDrawn)
		{
			SetPlayerSpeed(WeaponDrawnMultiplier);
			SetPlayerMovementType(false, true);
		}
		else
		{
			SetPlayerSpeed(1.f);
		}
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
	if (!bWeaponIsDrawn) { PlayerInventory->HolsterUnholster(); }
	bIsAiming = true;
	AimZoom(true); // Call BP timeline, playing forward
	SetPlayerSpeed(AimMultiplier);

	if (PlayerInventory->CurrentWeapon) // Give weapon bonus accuracy
	{
		PlayerInventory->CurrentWeapon->bAimingBonus = true;
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
			SetPlayerSpeed(WeaponDrawnMultiplier);
		}
		else
		{
			SetPlayerSpeed(1.f);
		}

		if (PlayerInventory->CurrentWeapon) // Remove weapon bonus accuracy
		{
			PlayerInventory->CurrentWeapon->bAimingBonus = false;
		}
	}
}

void APlayerCharacter::DoubleJump()
{
	if (!GetCharacterMovement()->IsFalling() && !bIsRolling)
	{
		bCanDoubleJump = true;
		Jump();
	}
	else if (bCanDoubleJump)
	{
		bCanDoubleJump = false;
		bIsDoubleJumping = true;
		GetCharacterMovement()->Velocity.Z = 0.f;
		GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * 65000.f);
		GetCharacterMovement()->AirControl = 1.f;
		GetWorldTimerManager().SetTimer(DoubleJumpTimerHandle, this, &APlayerCharacter::ResetAirControl, .8f, false, .8f);
	}	
}

void APlayerCharacter::ResetAirControl()
{
	bIsDoubleJumping = false;
	GetCharacterMovement()->AirControl = 0.2f;
}

void APlayerCharacter::StartRoll()
{
	if (!GetCharacterMovement()->IsFalling() && !bIsRolling && CurrentStamina >= RollStaminaCost)
	{
		bIsRolling = true;
		if (bIsSprinting) { SprintEnd(); }
		if (PlayerInventory->CurrentWeapon) { PlayerInventory->CurrentWeapon->InterruptReload(); }
		IncrementStamina(-RollStaminaCost);

		// Calculate dodge direction
		FVector RollDirection = GetCharacterMovement()->Velocity;
		RollDirection.Z = 0.f;
		RollDirection.Normalize();
		if (RollDirection.Size() < 0.01f) // If not moving roll forward
		{
			RollDirection = RootComponent->GetForwardVector();
		}

		// Set roll speed
		SetPlayerSpeed(3.f);
		bool bLastOrientRotationToMovement = GetCharacterMovement()->bOrientRotationToMovement;
		SetPlayerMovementType(true, false);

		// Set timers
		FTimerDelegate RollTimerDel;
		RollTimerDel.BindUFunction(this, FName("Roll"), RollDirection, bLastOrientRotationToMovement);
		GetWorldTimerManager().SetTimer(DodgeTimerHandle, RollTimerDel, 1.f / 120.f, true);
		FTimerDelegate RollEndTimerDel;
		RollEndTimerDel.BindUFunction(this, FName("EndRoll"), bLastOrientRotationToMovement);
		GetWorldTimerManager().SetTimer(DodgeEndTimerHandle, RollEndTimerDel, .75f, false);
	}
}

void APlayerCharacter::Roll(FVector DodgeDirection, bool bLastOrientRotationToMovement)
{
	AddMovementInput(DodgeDirection, 1.f);

	if (GetCharacterMovement()->IsFalling())
	{
		EndRoll(bLastOrientRotationToMovement);
	}
}

void APlayerCharacter::EndRoll(bool bLastOrientRotationToMovement)
{
	// Reset movement	
	SetPlayerSpeed(1.f);
	SetPlayerMovementType(bLastOrientRotationToMovement, !bLastOrientRotationToMovement);

	// Clear roll timers
	GetWorldTimerManager().ClearTimer(DodgeTimerHandle);
	GetWorldTimerManager().ClearTimer(DodgeEndTimerHandle);

	bIsRolling = false;
}

void APlayerCharacter::EnableCollsion()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APlayerCharacter::SetPlayerSpeed(float SpeedMultiplier)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
	}
}


void APlayerCharacter::SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = bOrientRotationToMovement;
		CharacterMovement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
	}
}

bool APlayerCharacter::Interact_Implementation()
{
	AActor* ActorHit = nullptr;
	if (InteractTrace(ActorHit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *GetNameSafe(ActorHit));
		
		if (ABaseVehicle* Vehicle = Cast<ABaseVehicle>(ActorHit)) // If a vehicle, possess this vehicle pawn
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
