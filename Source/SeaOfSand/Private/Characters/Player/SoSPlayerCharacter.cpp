// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerCharacter.h"
#include "SoSPlayerController.h"
#include "SoSASComponent.h"
#include "SoSASAbilityBase.h"
#include "SoSInventoryComponent.h"
#include "BaseVehicle.h"
#include "SoSRangedWeapon.h"
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
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Setup inventory
	InventoryComp = CreateDefaultSubobject<USoSInventoryComponent>(TEXT("PlayerInventory"));

	// Setup health component
	ASComp = CreateDefaultSubobject<USoSASComponent>(TEXT("ASComp"));

	// Setup camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 40.f);

	// Setup camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Configure character movement 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = ASComp->GetASAttributeTotalValue(EASAttributeName::Speed);
	GetCharacterMovement()->AirControl = 0.2f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;	

	// Configure interact
	InteractTraceRange = 250.f;

	// Set bools
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

	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityOne", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityOne);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityTwo", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityTwo);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityThree", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityThree);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFour", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityFour);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFive", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityFive);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySix", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilitySix);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySeven", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilitySeven);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityEight", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, AbilityBar.AbilityEigth);
}

// Called when the game starts or when spawned
void ASoSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Get controller
	PlayerController = Cast<ASoSPlayerController>(GetController());
}


void ASoSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ASComp->GetASOwnerState() == EASOwnerState::Dashing)
	{
		//Roll(RollDirection, bLastOrientRotationToMovement);
	}
}


void ASoSPlayerCharacter::MoveForward(float AxisValue)
{
	if (ASComp->GetASOwnerState() == EASOwnerState::Dashing || AxisValue == 0.0f)
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
	if (ASComp->GetASOwnerState() == EASOwnerState::Dashing || AxisValue == 0.0f)
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
	if (ASComp->GetASOwnerState() == EASOwnerState::Dashing || GetCharacterMovement()->IsFalling() || GetVelocity().Size() < 0.01f)
	{
		return;
	}

	if (UseAbility(AbilityBar.AbilitySprint))
	{
	AimEnd();
	SprintZoom(true);
	ASComp->SetASOwnerState(EASOwnerState::Sprinting);
	}

	//if (InventoryComp->GetCurrentWeapon()) { InventoryComp->GetCurrentWeapon()->InterruptReload(); }

	if (InventoryComp->GetCurrentWeapon())
	{
		SetPlayerMovementType(true, false);
	}
}

void ASoSPlayerCharacter::SprintEnd()
{
	if (ASComp->GetASOwnerState() != EASOwnerState::Sprinting)
	{
		return;
	}

	if (UseAbility(AbilityBar.AbilitySprintEnd))
	{
	SprintZoom(false);
	ASComp->SetASOwnerState(EASOwnerState::Normal);
	}
	
	if (InventoryComp->GetCurrentWeapon()->GetWeaponState() != EWeaponState::Holstered)
	{
		SetPlayerMovementType(false, true);
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
	if (InventoryComp->GetCurrentWeapon()->GetWeaponState() == EWeaponState::Holstered)
	{
		InventoryComp->HolsterUnholster();
	}

	if (UseAbility(InventoryComp->GetCurrentWeapon()->GetWeaponAbilities().AbilityWeaponAlt))
	{
	SprintEnd();
	AimZoom(true);
	ASComp->SetASOwnerState(EASOwnerState::Aiming);
	}	
	
	/*
	if (InventoryComp->GetCurrentWeapon()) // Give weapon bonus accuracy
	{
		//InventoryComp->GetCurrentWeapon()->SetGettingAccuracyBonus(true);
	} */
}

void ASoSPlayerCharacter::AimEnd()
{
	if (ASComp->GetASOwnerState() != EASOwnerState::Aiming)
	{
		return;
	}

	if (UseAbility(AbilityBar.AbilityAimEnd))
	{
	AimZoom(false);
	ASComp->SetASOwnerState(EASOwnerState::Normal);
	}
	
	/*		
	if (InventoryComp->GetCurrentWeapon()) // Remove weapon bonus accuracy
	{
		//InventoryComp->GetCurrentWeapon()->SetGettingAccuracyBonus(false);
	} */
}

void ASoSPlayerCharacter::DoubleJump()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCanDoubleJump = true;
		Jump();
	}
	else if (bCanDoubleJump)
	{
		bCanDoubleJump = false;
		bIsDoubleJumping = true;
		GetCharacterMovement()->Velocity.Z = 0.f;
		GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, 1.f) * 60000.f);
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
	/*
	if (GetCharacterMovement()->IsFalling() && bIsRolling)
	{
		return;
	}

	// End sprint and reloading
	if (bIsSprinting) { SprintEnd(); }
	//if (ASoSRangedWeapon* CurrentWeapon = InventoryComp->GetCurrentWeapon()) { CurrentWeapon->InterruptReload(); }

	// End aiming bonus if aiming
	if (InventoryComp->GetCurrentWeapon() && bIsAiming)
	{
		//InventoryComp->GetCurrentWeapon()->SetGettingAccuracyBonus(false);
	}
	
	bIsRolling = true;
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
	*/
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
	/*
	bIsRolling = false;

	// Re-enable aiming bonus if aiming
	if (InventoryComp->GetCurrentWeapon() && bIsAiming)
	{
		//InventoryComp->GetCurrentWeapon()->SetGettingAccuracyBonus(true);
	}

	// Reset movement	
	SetPlayerSpeed(1.f);
	SetPlayerMovementType(bLastOrientRotationToMovement, !bLastOrientRotationToMovement); */
}


void ASoSPlayerCharacter::UseAbilityActionBinding(TSubclassOf<USoSASAbilityBase> Ability)
{
	UseAbility(Ability);
}


bool ASoSPlayerCharacter::UseAbility(TSubclassOf<USoSASAbilityBase> Ability)
{
	if (Ability == nullptr) 
	{
		return false;
	}

	return ASComp->UseAbility(NewObject<USoSASAbilityBase>(AbilityBar.AbilityOne, Ability.Get()));
}


void ASoSPlayerCharacter::EnableCollsion()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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


/////////////////////////
/* Getters and Setters */
/////////////////////////


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


ASoSPlayerController * ASoSPlayerCharacter::GetPlayerController() const
{
	return PlayerController;
}


USoSInventoryComponent * ASoSPlayerCharacter::GetPlayerInventory() const
{
	return InventoryComp;
}


USoSASComponent* ASoSPlayerCharacter::GetPlayerASComponent() const
{
	return ASComp;
}


void ASoSPlayerCharacter::SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = bOrientRotationToMovement;
		CharacterMovement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
	}
}
