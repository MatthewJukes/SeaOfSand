// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerCharacter.h"
#include "SoSPlayerController.h"
#include "SoSASTasks.h"
#include "SoSCombatComponent.h"
#include "SoSAbilityBase.h"
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
	CombatComp = CreateDefaultSubobject<USoSCombatComponent>(TEXT("CombatComp"));
	CombatComp->SetAimHitLocation(&AimHitLocation);

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
	GetCharacterMovement()->MaxWalkSpeed = CombatComp->GetAttributeTotalValue(EAttributeName::Speed);
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
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ASoSPlayerCharacter::StartDash);

	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityOne", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 1);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityTwo", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 2);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityThree", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 3);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFour", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 4);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFive", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 5);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySix", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 6);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySeven", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 7);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityEight", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 8);
}

// Called when the game starts or when spawned
void ASoSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Get controller
	PlayerController = Cast<ASoSPlayerController>(GetController());

	AbilityBar.AbilityOne = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityOneClass, CombatComp);
	AbilityBar.AbilityTwo = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityTwoClass, CombatComp);
	AbilityBar.AbilityThree = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityThreeClass, CombatComp);
	AbilityBar.AbilityFour = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityFourClass, CombatComp);
	AbilityBar.AbilityFive = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityFiveClass, CombatComp);
	AbilityBar.AbilitySix = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySixClass, CombatComp);
	AbilityBar.AbilitySeven = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySevenClass, CombatComp);
	AbilityBar.AbilityEight = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityEightClass, CombatComp);
	AbilityBar.AbilitySprint = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySprintClass, CombatComp);
	AbilityBar.AbilitySprintEnd = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySprintEndClass, CombatComp);
	AbilityBar.AbilityAimEnd = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityAimEndClass, CombatComp);
	AbilityBar.AbilityDash = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityDashClass, CombatComp);
}


void ASoSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimHitLocation = PlayerController->GetCrosshairHitLocation(true, GetActorLocation());

	if (!GetCharacterMovement()->IsFalling())
	{
		bCanAirDash = true;
		bCanDoubleJump = true;
	}
}


void ASoSPlayerCharacter::MoveForward(float AxisValue)
{
	if (AxisValue == 0.0f)
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
	if (AxisValue == 0.0f)
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
	if (GetCharacterMovement()->IsFalling() || GetVelocity().Size() < 0.01f)
	{
		return;
	}

	if (UseAbility(AbilityBar.AbilitySprint))
	{
		AimEnd();
		SprintZoom(true);
		CombatComp->SetOwnerState(EOwnerState::Sprinting);
		
		if (InventoryComp->GetCurrentWeapon()->GetWeaponType() == EWeaponType::Ranged)
		{
			SetPlayerMovementType(true, false);
		}
	}

	//if (InventoryComp->GetCurrentWeapon()) { InventoryComp->GetCurrentWeapon()->InterruptReload(); }

}


void ASoSPlayerCharacter::SprintEnd()
{
	if (CombatComp->GetOwnerState() != EOwnerState::Sprinting)
	{
		return;
	}

	if (UseAbility(AbilityBar.AbilitySprintEnd))
	{
		SprintZoom(false);
		CombatComp->SetOwnerState(EOwnerState::Normal);
	
		if (InventoryComp->GetCurrentWeapon()->GetWeaponState() != EWeaponState::Holstered && InventoryComp->GetCurrentWeapon()->GetWeaponType() == EWeaponType::Ranged)
		{
			SetPlayerMovementType(false, true);
		}
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
		CombatComp->SetOwnerState(EOwnerState::Aiming);
	}	
}


void ASoSPlayerCharacter::AimEnd()
{
	if (CombatComp->GetOwnerState() != EOwnerState::Aiming)
	{
		return;
	}
	
	if (UseAbility(AbilityBar.AbilityAimEnd))
	{
		AimZoom(false);
		CombatComp->SetOwnerState(EOwnerState::Normal);
	}
}


void ASoSPlayerCharacter::DoubleJump()
{
	if (!GetCharacterMovement()->IsFalling())
	{
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


void ASoSPlayerCharacter::StartDash()
{
	if (!bCanAirDash)
	{
		return;
	}

	bCanAirDash = false;
	UseAbility(AbilityBar.AbilityDash);
}


void ASoSPlayerCharacter::UseAbilityActionBinding(int32 index)
{
	switch (index)
	{
	case 1:
		UseAbility(AbilityBar.AbilityOne);
		break;
	case 2:
		UseAbility(AbilityBar.AbilityTwo);
		break;
	case 3:
		UseAbility(AbilityBar.AbilityThree);
		break;
	case 4:
		UseAbility(AbilityBar.AbilityFour);
		break;
	case 5:
		UseAbility(AbilityBar.AbilityFive);
		break;
	case 6:
		UseAbility(AbilityBar.AbilitySix);
		break;
	case 7:
		UseAbility(AbilityBar.AbilitySeven);
		break;
	case 8:
		UseAbility(AbilityBar.AbilityEight);
		break;
	default:
		break;
	}
}


bool ASoSPlayerCharacter::UseAbility(USoSAbilityBase* Ability)
{
	return CombatComp->UseAbility(Ability);
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


USoSCombatComponent* ASoSPlayerCharacter::GetPlayerCombatComponent() const
{
	return CombatComp;
}


FPlayerAbilitiesData& ASoSPlayerCharacter::GetASAbilityBar()
{
	return AbilityBar;
}

void ASoSPlayerCharacter::SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation)
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = bOrientRotationToMovement;
		CharacterMovement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
	}
}
