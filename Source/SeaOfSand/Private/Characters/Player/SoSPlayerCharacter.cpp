// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerCharacter.h"
#include "PlayerInventory.h"
#include "SoSPlayerController.h"
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
ASoSPlayerCharacter::ASoSPlayerCharacter()
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
	MaxStamina = 100.f;
	BaseStaminaRegenRate = 6.f;
	SprintStaminaDrainRate = 10.f;
	RollStaminaCost = 20.f;

	// Configure character movement
	BaseSpeed = 400.f;
	SprintMultiplier = 1.6f;
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
	bIsRolling = false;
	bIsSprinting = false;
	bIsDoubleJumping = false;
	bCanDoubleJump = true;
	bInVehicle = false;
}

// Bind Inputs specific to player on foot
void ASoSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASoSPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASoSPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ASoSPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASoSPlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("USe", IE_Pressed, this, &ASoSPlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASoSPlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASoSPlayerCharacter::SprintEnd);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASoSPlayerCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASoSPlayerCharacter::CrouchEnd);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASoSPlayerCharacter::AimStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASoSPlayerCharacter::AimEnd);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASoSPlayerCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ASoSPlayerCharacter::StartRoll);
}

// Called when the game starts or when spawned
void ASoSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Get controller
	PlayerController = Cast<ASoSPlayerController>(GetController());

	// Set default health and stamina values
	CurrentStamina = MaxStamina;
	SetStaminaRate(BaseStaminaRegenRate);
}

void ASoSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRolling)
	{
		Roll(RollDirection, bLastOrientRotationToMovement);
	}
}

void ASoSPlayerCharacter::SetStaminaRate(float RatePerSecond)
{
	float TickRate = 0.25f;
	
	// Set timers
	FTimerDelegate StaminaTimerDel;
	StaminaTimerDel.BindUFunction(this, FName("IncrementStamina"), RatePerSecond * TickRate);
	GetWorldTimerManager().SetTimer(TimerHandle_Stamina, StaminaTimerDel, TickRate, true);
}

void ASoSPlayerCharacter::IncrementStamina(float Amount)
{
	CurrentStamina += Amount;
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);

	if (CurrentStamina <= 0.1f)
	{
		if (bIsSprinting) { SprintEnd(); }
	}
}

void ASoSPlayerCharacter::MoveForward(float AxisValue)
{
	if (bIsRolling || AxisValue == 0.0f)
	{
		return;
	}

	if (Controller)
	{
		// Find out which was is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void ASoSPlayerCharacter::MoveRight(float AxisValue)
{
	if (bIsRolling || AxisValue == 0.0f)
	{
		return;
	}

	if (Controller)
	{
		// Find out which was is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void ASoSPlayerCharacter::SprintStart()
{
	if (bIsRolling || GetCharacterMovement()->IsFalling() || CurrentStamina <= 5.0f || GetVelocity().Size() < 0.01f)
	{
		return;
	}
	
	// End aiming and reloading
	if (bIsAiming) { AimEnd(); }
	if (PlayerInventory->CurrentWeapon) { PlayerInventory->CurrentWeapon->InterruptReload(); }

	bIsSprinting = true;
	SprintZoom(true);
	SetStaminaRate(-SprintStaminaDrainRate);

	if (PlayerInventory->bWeaponIsDrawn)
	{
		SetPlayerSpeed(PlayerInventory->CurrentWeapon->WeaponDrawnSpeedMultiplier * SprintMultiplier);
		SetPlayerMovementType(true, false);
	}
	else
	{
		SetPlayerSpeed(SprintMultiplier);
	}
}

void ASoSPlayerCharacter::SprintEnd()
{
	if (!bIsSprinting)
	{
		return;
	}

	bIsSprinting = false;
	SprintZoom(false);
	SetStaminaRate(BaseStaminaRegenRate);

	if (PlayerInventory->bWeaponIsDrawn)
	{
		SetPlayerSpeed(PlayerInventory->CurrentWeapon->WeaponDrawnSpeedMultiplier);
		SetPlayerMovementType(false, true);
	}
	else
	{
		SetPlayerSpeed(1.f);
	}	
}

void ASoSPlayerCharacter::CrouchStart()
{
	// TODO add player crouching
}

void ASoSPlayerCharacter::CrouchEnd()
{
}

void ASoSPlayerCharacter::AimStart()
{
	// End sprint and reloading
	if (bIsSprinting) { SprintEnd(); }
	if (!PlayerInventory->bWeaponIsDrawn) { PlayerInventory->HolsterUnholster(); }

	bIsAiming = true;
	AimZoom(true);
	SetPlayerSpeed(PlayerInventory->CurrentWeapon->AimingSpeedMultiplier);

	if (PlayerInventory->CurrentWeapon) // Give weapon bonus accuracy
	{
		PlayerInventory->CurrentWeapon->bAimingBonus = true;
	}
}

void ASoSPlayerCharacter::AimEnd()
{
	if (!bIsAiming)
	{
		return;
	}

	bIsAiming = false;
	AimZoom(false);

	if (PlayerInventory->bWeaponIsDrawn)
	{
		SetPlayerSpeed(PlayerInventory->CurrentWeapon->WeaponDrawnSpeedMultiplier);
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

void ASoSPlayerCharacter::DoubleJump()
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
		GetWorldTimerManager().SetTimer(TimerHandle_DoubleJump, this, &ASoSPlayerCharacter::ResetAirControl, .8f, false, .8f);
	}	
}

void ASoSPlayerCharacter::ResetAirControl()
{
	bIsDoubleJumping = false;
	GetCharacterMovement()->AirControl = 0.2f;
}

void ASoSPlayerCharacter::StartRoll()
{
	if (GetCharacterMovement()->IsFalling() && bIsRolling && CurrentStamina < RollStaminaCost)
	{
		return;
	}

	// End sprint and reloading
	if (bIsSprinting) { SprintEnd(); }
	if (PlayerInventory->CurrentWeapon) { PlayerInventory->CurrentWeapon->InterruptReload(); }
	
	bIsRolling = true;
	IncrementStamina(-RollStaminaCost);
	bLastOrientRotationToMovement = GetCharacterMovement()->bOrientRotationToMovement;

	// Calculate dodge direction
	RollDirection = GetCharacterMovement()->Velocity;
	RollDirection.Z = 0.f;
	RollDirection.Normalize();
	if (RollDirection.Size() < 0.01f) // If not moving roll forward
	{
		RollDirection = RootComponent->GetForwardVector();
	}

	// Set roll speed
	SetPlayerSpeed(3.f);
	SetPlayerMovementType(true, false);

	// Set timers
	FTimerDelegate RollEndTimerDel;
	RollEndTimerDel.BindUFunction(this, FName("EndRoll"), bLastOrientRotationToMovement);
	GetWorldTimerManager().SetTimer(TimerHandle_DodgeEnd, RollEndTimerDel, .75f, false);
}

void ASoSPlayerCharacter::Roll(FVector DodgeDirection, bool bLastOrientRotationToMovement)
{
	AddMovementInput(DodgeDirection, 1.f);

	if (GetCharacterMovement()->IsFalling())
	{
		EndRoll(bLastOrientRotationToMovement);
	}
}

void ASoSPlayerCharacter::EndRoll(bool bLastOrientRotationToMovement)
{
	bIsRolling = false;

	// Reset movement	
	SetPlayerSpeed(1.f);
	SetPlayerMovementType(bLastOrientRotationToMovement, !bLastOrientRotationToMovement);
}

void ASoSPlayerCharacter::EnableCollsion()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASoSPlayerCharacter::SetPlayerSpeed(float SpeedMultiplier)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
	}
}

void ASoSPlayerCharacter::SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = bOrientRotationToMovement;
		CharacterMovement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
	}
}

void ASoSPlayerCharacter::Interact()
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
}

bool ASoSPlayerCharacter::InteractTrace(AActor* &OutActor) const
{
	const FName TraceTag("InteractTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("TraceParams")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	FHitResult Hit;
	FVector StartLocation = GetActorLocation() + FVector(0.f,0.f,40.f);
	FVector EndLocation = StartLocation + (GetTraceDirection(StartLocation) * InteractTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{		
		OutActor = Hit.GetActor();
		return true;
	}
	return false; // Trace didn't hit anything
}

FVector ASoSPlayerCharacter::GetTraceDirection(FVector StartLocation) const
{
	if (PlayerController)
	{
		FVector TraceDirection = PlayerController->GetCrosshairHitLocation() - StartLocation;
		TraceDirection.Normalize();
		return TraceDirection;
	}
	return FVector(0.f,0.f,0.f);
}