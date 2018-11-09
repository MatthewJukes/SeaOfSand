// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPlayerCharacter.h"
#include "SoSPlayerController.h"
#include "SoSASTasks.h"
#include "SoSCombatComponent.h"
#include "SoSAbilityBase.h"
#include "SoSPlayerAbilityTarget.h"
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

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ASoSPlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASoSPlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASoSPlayerCharacter::SprintEnd);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASoSPlayerCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASoSPlayerCharacter::CrouchEnd);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASoSPlayerCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ASoSPlayerCharacter::DashStart);

	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityOne", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 1, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityTwo", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 2, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityThree", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 3, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFour", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 4, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFive", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 5, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySix", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 6, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySeven", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 7, false);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityEight", IE_Pressed, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 8, false);

	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityOne", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 1, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityTwo", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 2, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityThree", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 3, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFour", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 4, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityFive", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 5, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySix", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 6, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilitySeven", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 7, true);
	PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbilityEight", IE_Released, this, &ASoSPlayerCharacter::UseAbilityActionBinding, 8, true);
}

// Called when the game starts or when spawned
void ASoSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Get controller
	PlayerController = Cast<ASoSPlayerController>(GetController());

	InventoryComp->OnWeaponSwitch.AddDynamic(this, &ASoSPlayerCharacter::TargetingModeEnd);

	AbilityTarget = GetWorld()->SpawnActor<ASoSPlayerAbilityTarget>(BP_AbilityTarget, GetActorTransform());
	if (AbilityTarget)
	{
		AbilityTarget->SetPlayerCharacter(this);
	}

	AbilityBar.AbilitySprint = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySprintClass, CombatComp);
	AbilityBar.AbilitySprintEnd = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilitySprintEndClass, CombatComp);
	AbilityBar.AbilityAimEnd = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityAimEndClass, CombatComp);
	AbilityBar.AbilityDash = USoSASTasks::CreateAbilityInstance(AbilityBar.AbilityDashClass, CombatComp);
}


void ASoSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerController->GetCrosshairHitResult(AimHitResult, true, GetActorLocation());

	if (AbilityTarget && bTargetingModeActive)
	{
		TargetedLocation = AbilityTarget->GetTargetLocation();
	}

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
			SetCharacterMovementType(true, false);
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
			SetCharacterMovementType(false, true);
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


void ASoSPlayerCharacter::DashStart()
{
	if (!bCanAirDash)
	{
		return;
	}

	bCanAirDash = false;
	UseAbility(AbilityBar.AbilityDash);
}


void ASoSPlayerCharacter::TargetingModeStart()
{
	bTargetingModeActive = true;
	
	if (AbilityTarget == nullptr)
	{
		return;
	}

	AbilityTarget->SetTargetRadius(CurrentAbilityTargeting->GetTargetRadius());
	AbilityTarget->SetMaxTargetRange(CurrentAbilityTargeting->GetMaxRange());
	AbilityTarget->SetSnapToGround(CurrentAbilityTargeting->GetSnapToGround());
	AbilityTarget->Activate(CurrentAbilityTargeting->GetTargetingShape());
}


void ASoSPlayerCharacter::TargetingModeEnd()
{
	if (!bTargetingModeActive)
	{
		return;
	}

	bTargetingModeActive = false;

	if (AbilityTarget == nullptr)
	{
		return;
	}

	AbilityTarget->Deactivate();
}


void ASoSPlayerCharacter::PrimaryAttackStart()
{
	if (bTargetingModeActive)
	{
		if (CurrentAbilityTargeting == nullptr)
		{
			return;
		}

		if (CurrentAbilityTargeting->GetCastType() == EAbilityCastType::Aimed)
		{
			CombatComp->UseAbility(CurrentAbilityTargeting, false, ClassSpecificFloat);
		}

		return;
	}


	if (InventoryComp)
	{
		if (InventoryComp->GetCurrentWeapon()->GetWeaponState() == EWeaponState::Holstered) // Draw weapon if not drawn already
		{
			InventoryComp->HolsterUnholster();
		}
		else
		{
			SprintEnd();
			InventoryComp->GetCurrentWeapon()->StartAttack();
		}
	}
}


void ASoSPlayerCharacter::PrimaryAttackEnd()
{
	if (bTargetingModeActive)
	{
		if (CurrentAbilityTargeting == nullptr)
		{
			return;
		}

		CombatComp->UseAbility(CurrentAbilityTargeting, true, ClassSpecificFloat);
		TargetingModeEnd();
		return;
	}

	if (InventoryComp)
	{
		InventoryComp->GetCurrentWeapon()->EndAttack();
	}
}


void ASoSPlayerCharacter::AlternateAttackStart()
{
	if (bTargetingModeActive)
	{
		return;
	}

	AimStart();
}


void ASoSPlayerCharacter::AlternateAttackEnd()
{
	if (bTargetingModeActive)
	{
		TargetingModeEnd();
	}

	AimEnd();
}


void ASoSPlayerCharacter::UseAbilityActionBinding(int32 index, bool bReleased)
{
	USoSAbilityBase* AbilityToUse;
	bool bMeleeWeapon = InventoryComp->GetCurrentWeapon()->GetWeaponType() == EWeaponType::Melee ? true : false;

	switch (index)
	{
	case 1:
		AbilityToUse = bMeleeWeapon ? AbilityBar.MeleeAbilityOne : AbilityBar.RangedAbilityOne;
		break;
	case 2:
		AbilityToUse = bMeleeWeapon ? AbilityBar.MeleeAbilityTwo : AbilityBar.RangedAbilityTwo;
		break;
	case 3:
		AbilityToUse = bMeleeWeapon ? AbilityBar.MeleeAbilityThree : AbilityBar.RangedAbilityThree;
		break;
	case 4:
		AbilityToUse = AbilityBar.OffhandAbilityOne;
		break;
	case 5:
		AbilityToUse = AbilityBar.OffhandAbilityTwo;
		break;
	case 6:
		AbilityToUse = AbilityBar.AuxAbilityOne;
		break;
	case 7:
		AbilityToUse = AbilityBar.AuxAbilityTwo;
		break;
	case 8:
		AbilityToUse = AbilityBar.AuxAbilityThree;
		break;
	default:
		AbilityToUse = nullptr;
		break;
	}

	UseAbility(AbilityToUse, bReleased);
}


bool ASoSPlayerCharacter::UseAbility(USoSAbilityBase* Ability, bool bReleased)
{
	if (Ability == nullptr)
	{
		return false;
	}

	if (!CombatComp->CheckAbilityCanCast(Ability))
	{
		return false;
	}

	if (bTargetingModeActive && Ability->GetCastType() != EAbilityCastType::Instant)
	{
		TargetingModeEnd();
	}

	if (Ability->GetCastType() == EAbilityCastType::Default || Ability->GetCastType() == EAbilityCastType::Instant)
	{
		return CombatComp->UseAbility(Ability, bReleased, ClassSpecificFloat);
	}
	else if (Ability->GetCastType() == EAbilityCastType::AimedCharge && bReleased == true)
	{
		CurrentAbilityTargeting = Ability;
		TargetingModeStart();
		return CombatComp->UseAbility(Ability, false, ClassSpecificFloat);
	}
	else if (Ability->GetCastType() == EAbilityCastType::Aimed)
	{
		CurrentAbilityTargeting = Ability;
		TargetingModeStart();
		return true;
	}

	return false;
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
		FHitResult Hit;
		PlayerController->GetCrosshairHitResult(Hit);
		FVector TraceDirection = Hit.Location - StartLocation;
		TraceDirection.Normalize();
		return TraceDirection;
	}
	return FVector(0.f,0.f,0.f);
}


ASoSPlayerController * ASoSPlayerCharacter::GetPlayerController() const
{
	return PlayerController;
}


UCameraComponent* ASoSPlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}


FPlayerAbilitiesData& ASoSPlayerCharacter::GetASAbilityBar()
{
	return AbilityBar;
}


bool ASoSPlayerCharacter::GetIsDoubleJumping() const
{
	return bIsDoubleJumping;
}


bool ASoSPlayerCharacter::GetInVehicle() const
{
	return bInVehicle;
}


bool ASoSPlayerCharacter::GetTargetingModeActive() const
{
	return bTargetingModeActive;
}


void ASoSPlayerCharacter::SetInVehicle(bool NewBool)
{
	bInVehicle = NewBool;
}
