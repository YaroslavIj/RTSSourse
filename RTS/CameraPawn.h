// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
//
#include "CameraPawn.generated.h"

UCLASS()
class RTS_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* Scene = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MovementSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float CameraVerticalSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxCameraHeight = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MinCameraHeight = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float CameraVerticalOffset = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxCameraMoveDistance = 3000.f;
	FVector DefaultCameraLocation;

	float TargetCameraHeight;

	float MaxUnitsMoveDistance;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector HighlightingStartMouseLocation;
	bool bIsLeftMousePressed = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void OnLeftMouseReleased();
	UFUNCTION()
	void OnRightMouseClicked();
	UFUNCTION()
	void OnLeftMousePressed();
	
	void TickRectangleHighlighting();
	void TickCameraMove(float DeltaTime);
	UFUNCTION()
	void OnMouseWheelUp();
	UFUNCTION()
	void OnMouseWheelDown();


};
