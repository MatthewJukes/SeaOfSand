// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVehicle.h"
#include "DesertSpeeder.generated.h"

class UHoverComponent;

UCLASS()
class SEAOFSANDS_API ADesertSpeeder : public ABaseVehicle
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent4;

public:
	ADesertSpeeder();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
