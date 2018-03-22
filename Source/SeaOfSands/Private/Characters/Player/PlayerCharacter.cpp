// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "BasePlayerController.h"
#include "BaseWeapon.h"
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
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::AimStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::AimEnd);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::JumpAndFlip);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	HolsterWeapon();
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

void APlayerCharacter::AimStart()
{
	if (bIsSprinting) { SprintEnd(); }
	bIsAiming = true;
	EquipWeapon();
	AimZoom(true); // Call BP timeline, playing forward
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * AimMultiplier;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void APlayerCharacter::AimEnd()
{
	if (bIsAiming) // Only execute if aiming
	{
		bIsAiming = false;
		HolsterWeapon();
		AimZoom(false); // Call BP timeline, playing backwards
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
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
	//CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(RifleBlueprint, SpawnParams);

	//ABasePlayerController* Controller = Cast<ABasePlayerController>(GetController());
	//Controller->UpdateCurrentWeapon(CurrentWeapon);
}

void APlayerCharacter::EquipWeapon()
{
	if (CurrentWeapon)
	{
		//CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachPoint);
		bCanFire = true;
	}
}

void APlayerCharacter::HolsterWeapon()
{
	if (CurrentWeapon)
	{
		//CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, BackHolsterAttachPoint);
		//CurrentWeapon->StopFiring();
		bCanFire = false;
	}
}



