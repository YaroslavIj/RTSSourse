// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "EnemyUnit.h"
#include "AllyUnit.h"
#include "MainGM.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetCameraHeight = GetActorLocation().Z;
	DefaultCameraLocation = GetActorLocation();
	MaxUnitsMoveDistance = MaxCameraMoveDistance + 3000.f;
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickRectangleHighlighting();

	TickCameraMove(DeltaTime);
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftMouseClick", EInputEvent::IE_Released, this, &ACameraPawn::OnLeftMouseReleased);
	PlayerInputComponent->BindAction("LeftMouseClick", EInputEvent::IE_Pressed, this, &ACameraPawn::OnLeftMousePressed);
	PlayerInputComponent->BindAction("RightMouseClick", EInputEvent::IE_Pressed, this, &ACameraPawn::OnRightMouseClicked);
	PlayerInputComponent->BindAction("MouseWheelUp", EInputEvent::IE_Pressed, this, &ACameraPawn::OnMouseWheelUp);
	PlayerInputComponent->BindAction("MouseWheelDown", EInputEvent::IE_Pressed, this, &ACameraPawn::OnMouseWheelDown);
}

void ACameraPawn::OnLeftMouseReleased()
{
	bIsLeftMousePressed = false;
	if (GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery5, true, HitResult);
			float MouseMoveDistance = FVector::Dist2D(HighlightingStartMouseLocation, HitResult.ImpactPoint);
			if (!FMath::IsNearlyZero(MouseMoveDistance, 20))
			{
				FVector Difference = HitResult.ImpactPoint - HighlightingStartMouseLocation;
				FVector Center = HighlightingStartMouseLocation + Difference / 2;
				float SizeX = FMath::Abs(Difference.X / 2.f);
				float SizeY = FMath::Abs(Difference.Y / 2.f);
				TArray<AActor*> ActorsoIgnore;
				TArray<FHitResult> BoxHits;
				UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Center, Center, FVector(SizeX, SizeY, 100), FRotator(0),
					ETraceTypeQuery::TraceTypeQuery3, true, ActorsoIgnore, EDrawDebugTrace::None, BoxHits, true);
				for (FHitResult BoxHit : BoxHits)
				{
					AActor* HitActor = BoxHit.GetActor();
					if (GetWorld() && GetWorld()->GetAuthGameMode() && HitActor)
					{
						if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
						{
							if (HitActor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
							{
								if (AAllyUnit* AllyUnit = Cast<AAllyUnit>(HitActor))
								{
									if (!GM->GetSelectedAllyUnits().Contains(AllyUnit))
									{
										GM->SelectAllyUnit(AllyUnit);
									}
								}
							}
						}
					}
				}
			}
			else
			{
				PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, HitResult);
				if (HitResult.bBlockingHit && HitResult.GetActor())
				{
					AActor* HitActor = HitResult.GetActor();
					if (GetWorld() && GetWorld()->GetAuthGameMode() && HitActor)
					{
						if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
						{
							if (HitActor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
							{
								if (AAllyUnit* AllyUnit = Cast<AAllyUnit>(HitActor))
								{
									if (!GM->GetSelectedAllyUnits().Contains(AllyUnit))
									{
										GM->SelectAllyUnit(AllyUnit);
									}
									else
									{
										GM->UnselectAllyUnit(AllyUnit);
									}
								}
							}
						}
					}
				}
			}		
		}
	}
}

void ACameraPawn::OnLeftMousePressed()
{
	if (GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery5, true, HitResult);
			HighlightingStartMouseLocation = HitResult.ImpactPoint;
			bIsLeftMousePressed = true;
		}
	}

}

void ACameraPawn::TickRectangleHighlighting()
{
	if (bIsLeftMousePressed)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			AHUD* HUD = PlayerController->GetHUD();
			if (HUD)
			{
				FHitResult HitResult;
				FVector Start = HighlightingStartMouseLocation;
				PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery5, true, HitResult);
				FVector End = HitResult.ImpactPoint;
				float MouseMoveDistance = FVector::Dist2D(HighlightingStartMouseLocation, HitResult.ImpactPoint);
				if (!FMath::IsNearlyZero(MouseMoveDistance, 20))
				{
					HUD->Draw3DLine(FVector(Start.X, Start.Y, 10), FVector(Start.X, End.Y, 10), FColor::Green);
					HUD->Draw3DLine(FVector(Start.X, End.Y, 10), FVector(End.X, End.Y, 10), FColor::Green);
					HUD->Draw3DLine(FVector(End.X, End.Y, 10), FVector(End.X, Start.Y, 10), FColor::Green);
					HUD->Draw3DLine(FVector(End.X, Start.Y, 10), FVector(Start.X, Start.Y, 10), FColor::Green);
				}
			}
		}
	}
}

void ACameraPawn::TickCameraMove(float DeltaTime)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		float XPos;
		float YPos;
		PlayerController->GetMousePosition(XPos, YPos);
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			FVector Direction;
			
			if (FMath::IsNearlyEqual(XPos, ViewportSize.X, 20))
			{
				Direction += GetActorRightVector();
			}
			if (FMath::IsNearlyZero(XPos, 20))
			{
				Direction -= GetActorRightVector();
			}
			if (FMath::IsNearlyEqual(YPos, ViewportSize.Y, 20))
			{
				Direction -= GetActorForwardVector();
			}
			if (FMath::IsNearlyZero(YPos, 20))
			{
				Direction += GetActorForwardVector();
			}
			FVector DeltaLocation = Direction * MovementSpeed * DeltaTime;
			FVector NewLocation = GetActorLocation() + DeltaLocation;
			if (FMath::Abs(NewLocation.X - DefaultCameraLocation.X) > MaxCameraMoveDistance)
			{
				DeltaLocation.X = 0;
			}
			if (FMath::Abs(NewLocation.Y - DefaultCameraLocation.Y) > MaxCameraMoveDistance)
			{
				DeltaLocation.Y = 0;
			}
			AddActorWorldOffset(DeltaLocation);
		}
	}
	if(!FMath::IsNearlyEqual(GetActorLocation().Z, TargetCameraHeight, 10.f))
	{
		FVector WatchDirection = Camera->GetComponentRotation().Vector();
		if(TargetCameraHeight < GetActorLocation().Z)
		{
			AddActorWorldOffset(WatchDirection * CameraVerticalSpeed * DeltaTime);
		}
		else
		{
			AddActorWorldOffset(-WatchDirection * CameraVerticalSpeed * DeltaTime);
		}
	}
}

void ACameraPawn::OnMouseWheelUp()
{
	if (Camera)
	{
		if (FMath::IsNearlyEqual(GetActorLocation().Z, TargetCameraHeight, 10.f))
		{
			FVector WatchDirection = Camera->GetComponentRotation().Vector();
			FVector NewTargetLocation = GetActorLocation() + WatchDirection * CameraVerticalOffset;
			if (NewTargetLocation.Z >= MinCameraHeight)
			{
				TargetCameraHeight = NewTargetLocation.Z;
			}
		}
	}
}

void ACameraPawn::OnMouseWheelDown()
{
	if (Camera)
	{
		FVector WatchDirection = Camera->GetComponentRotation().Vector();
		FVector NewTargetLocation = GetActorLocation() - WatchDirection * CameraVerticalOffset;
		if (NewTargetLocation.Z <= MaxCameraHeight)
		{
			TargetCameraHeight = NewTargetLocation.Z;
		}
	}
}

void ACameraPawn::OnRightMouseClicked()
{
	if (GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, HitResult);
			if (HitResult.bBlockingHit && HitResult.GetActor())
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor)
				{
					if (GetWorld() && GetWorld()->GetAuthGameMode())
					{
						if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
						{
							if (HitActor->GetClass()->IsChildOf(AEnemyUnit::StaticClass()))
							{
								if (AEnemyUnit* EnemyUnit = Cast<AEnemyUnit>(HitActor))
								{
									GM->SetEnemyToAttackForSelectedAllies(EnemyUnit);
									GM->UnselectAllAllyUnits();
								}
							}
							else if (FMath::Abs(HitResult.ImpactPoint.X - DefaultCameraLocation.X) < MaxUnitsMoveDistance &&
							  FMath::Abs(HitResult.ImpactPoint.Y - DefaultCameraLocation.Y) < MaxUnitsMoveDistance)			
							{
								GM->SetSelectedAlliesTargetLocation(HitResult.ImpactPoint);
								GM->UnselectAllAllyUnits();
							}
						}
					}
				}
				
			}
		}
	}
}
