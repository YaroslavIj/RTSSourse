// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGM.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "EnemySquadManager.h"
#include "RTSGameInstance.h"

void AMainGM::BeginPlay()
{
	Super::BeginPlay();

	if(URTSGameInstance* GI = Cast<URTSGameInstance>(GetGameInstance()))
	{		
		FName TargetPointTag = FName("AllyUnitsSpawnLocation");
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), OutActors);
		for(AActor* TargetPoint : OutActors)
		{
			if(TargetPoint->Tags[0] == TargetPointTag)			
			{
				SpawnAllyUnits(GI->GetAllyUnitsToSpawn(), TargetPoint->GetActorLocation());
			}
		}
		// Add 20% of all units for each level
		EnemyUnitsNumToSpawn = EnemyUnitsNumToSpawn + GI->GetLevelNumber() * PercentOfAdditionalEnemyUnitsPerLevel * EnemyUnitsNumToSpawn;
		SpawnEnemyUnits(EnemyUnitsNumToSpawn);
	}

}

UEnemySquadManager* AMainGM::GetEnemySquadManager(int32 SquadIndex)
{
	if (EnemySquadManagers.IsValidIndex(SquadIndex))
	{
		return EnemySquadManagers[SquadIndex];
	}
	return nullptr;
}

void AMainGM::SpawnAllyUnits(TMap<TSubclassOf<AAllyUnit>, int32> UnitsToSpawn, FVector SpawnLocation)
{	
	float AllUnitsSquare = 0;
	for (TPair<TSubclassOf<AAllyUnit>, int32>& Pair : UnitsToSpawn)
	{
		TSubclassOf<AAllyUnit> UnitClass = Pair.Key;
		int32 Count = Pair.Value;
		if (UnitClass)
		{
			for (int32 i = 0; i < Count; i++)
			{
				if (AAllyUnit* Unit = Cast<AAllyUnit>(UnitClass->GetDefaultObject()))
				{
					if (Unit->GetCapsuleComponent())
					{
						float UnitRadius = Unit->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
						float UnitSquare = powf(UnitRadius, 2) * PI;
						AllUnitsSquare += UnitSquare;
					}
				}
			}
		}
	}
	AllUnitsSquare *= 3;
	float SpawnRadius = sqrtf(AllUnitsSquare / PI);
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if(URTSGameInstance* GI = Cast<URTSGameInstance>(GetGameInstance()))	
	{
		for (TPair<TSubclassOf<AAllyUnit>, int32>& Pair : UnitsToSpawn)
		{
			TSubclassOf<AAllyUnit> UnitClass = Pair.Key;
			int32 Count = Pair.Value;
			if (UnitClass)
			{
				if (AAllyUnit* UnitToSpawn = Cast<AAllyUnit>(UnitClass->GetDefaultObject()))
				{
					if (UnitToSpawn->GetCapsuleComponent())
					{
						float SpawnUnitRadius = UnitToSpawn->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
						for (int32 i = 0; i < Count; i++)
						{
							FNavLocation OutSpawnNavLocation;
							FVector UnitSpawnLocation;
							bool bIsSuccess = false;
							int32 j = 0;
							while (!bIsSuccess && j < 100)
							{
								if (NavSystem->GetRandomReachablePointInRadius(SpawnLocation, SpawnRadius, OutSpawnNavLocation))
								{
									UnitSpawnLocation = FVector(OutSpawnNavLocation.Location.X, OutSpawnNavLocation.Location.Y, SpawnLocation.Z);
									bIsSuccess = true;
									for (AAllyUnit* OtherUnit : AllyUnits)
									{
										float OtherUnitRadius = OtherUnit->GetCapsuleComponent()->GetScaledCapsuleRadius();
										FVector OtherUnitLocation = OtherUnit->GetActorLocation();
										float DistanceToOtherUnit = FVector::Distance(UnitSpawnLocation, OtherUnitLocation);
										if (DistanceToOtherUnit < SpawnUnitRadius + OtherUnitRadius)
										{
											bIsSuccess = false;
										}
									}
								}
								j++;
							}
							if (bIsSuccess)
							{
								if (UnitClass)
								{
									FActorSpawnParameters SpawnParams;
									SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
									AAllyUnit* SpawnedUnit = GetWorld()->SpawnActor<AAllyUnit>(UnitClass, UnitSpawnLocation, FRotator(0), SpawnParams);
									if (SpawnedUnit)
									{
										FUnitInfo OutInfo;
										if(GI->GetAllyUnitInfo(UnitClass, OutInfo))
										{
											SpawnedUnit->Init(OutInfo);
											AllyUnits.Add(SpawnedUnit);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("AMainGM::SpawnUnits - Unit hasn't found location to spawn"));
							}
						}
					}
				}
			}
		}
	}
}

void AMainGM::SpawnEnemyUnits(int32 UnitsCount)
{
	FName TargetPointTag = FName("EnemyUnitsSpawnLocation");
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundActors);
	int32 EnemySquadNumber = EnemyUnitsInSquads.Num() - 1;
	for (int32 i = 0; i < FoundActors.Num(); i++)
	{
		if (ATargetPoint* TargetPoint = Cast<ATargetPoint>(FoundActors[i]))
		{
			if (TargetPoint->Tags[0] == TargetPointTag)
			{
				float AllUnitsSquare = 0;
				TArray<TSubclassOf<AEnemyUnit>> UnitsToSpawn;
				int32 RandomUnitsCount = FMath::RandRange(UnitsCount / 1.5f, UnitsCount * 1.5f);
				for (int32 j = 0; j < RandomUnitsCount; j++)
				{
					//int32 RandomUnitIndex = FMath::RandRange(0, EnemyUnitClasses.Num() - 1);
					
					int32 k = 0;
					bool bIsSuccess = false;
					while (k < EnemyUnitClasses.Num() && !bIsSuccess)
					{
						if (EnemyUnitClasses.IsValidIndex(k) && EnemyUnitClasses[k])
						{
							if (AEnemyUnit* Unit = Cast<AEnemyUnit>(EnemyUnitClasses[k]->GetDefaultObject()))
							{
								float Random = FMath::FRand();
								if(Random < Unit->SpawnChance)
								{
									if (Unit->GetCapsuleComponent())
									{
										UnitsToSpawn.Add(EnemyUnitClasses[k]);
										float UnitRadius = Unit->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
										float UnitSquare = powf(UnitRadius, 2) * PI;
										AllUnitsSquare += UnitSquare;
										bIsSuccess = true;
									}
								}
							}
						}
						if (!bIsSuccess && k == EnemyUnitClasses.Num() - 1)
						{
							k = 0;
						}
						else
						{
							k++;
						}
					}
				}
				AllUnitsSquare *= 3;
				float SpawnRadius = sqrtf(AllUnitsSquare / PI);

				EnemySquadNumber++;
				if (!EnemyUnitsInSquads.IsValidIndex(EnemySquadNumber))
				{
					EnemyUnitsInSquads.Add(TArray<AEnemyUnit*>());
				}
				UEnemySquadManager* SquadManager = NewObject<UEnemySquadManager>(this);
				if(SquadManager)
				{
					EnemySquadManagers.Add(SquadManager);
					SquadManager->Init(EnemySquadNumber, this, EnemySquadFindTargetRate, EnemySquadFindTargetRadius);
				}
				
				FTransform TargetTransform = TargetPoint->GetActorTransform();
				for (TSubclassOf<AEnemyUnit> UnitClass : UnitsToSpawn)
				{
					if (AEnemyUnit* UnitToSpawn = Cast<AEnemyUnit>(UnitClass->GetDefaultObject()))
					{
						if (UnitToSpawn->GetCapsuleComponent())
						{
							float SpawnUnitRadius = UnitToSpawn->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
							FNavLocation OutSpawnNavLocation;
							FVector UnitSpawnLocation;
							bool bIsSuccess = false;
							int32 j = 0;
							while (!bIsSuccess && j < 100)
							{
								if (NavSystem->GetRandomReachablePointInRadius(TargetTransform.GetLocation(), SpawnRadius, OutSpawnNavLocation))
								{
									UnitSpawnLocation = FVector(OutSpawnNavLocation.Location.X, OutSpawnNavLocation.Location.Y, TargetTransform.GetLocation().Z);
									bIsSuccess = true;
									for (AEnemyUnit* OtherUnit : EnemyUnitsInSquads[EnemySquadNumber])
									{
										float OtherUnitRadius = OtherUnit->GetCapsuleComponent()->GetScaledCapsuleRadius();
										FVector OtherUnitLocation = OtherUnit->GetActorLocation();
										float DistanceToOtherUnit = FVector::Distance(UnitSpawnLocation, OtherUnitLocation);
										if (DistanceToOtherUnit < SpawnUnitRadius + OtherUnitRadius)
										{
											bIsSuccess = false;
										}
									}
								}
								j++;
							}
							if (bIsSuccess)
							{
								if (UnitClass)
								{
									FActorSpawnParameters SpawnParams;
									SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
									AEnemyUnit* SpawnedUnit = GetWorld()->SpawnActor<AEnemyUnit>(UnitClass, UnitSpawnLocation, TargetTransform.GetRotation().Rotator(), SpawnParams);
									if(SpawnedUnit)
									{
										SpawnedUnit->SquadNumber = EnemySquadNumber;
										EnemyUnitsInSquads[EnemySquadNumber].Add(SpawnedUnit);
									}
								}
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("AMainGM::SpawnUnits - Unit hasn't found location to spawn"));
							}
						}
					}
				}
				if (EnemySquadManagers.IsValidIndex(EnemySquadNumber) && EnemySquadManagers[EnemySquadNumber])
				{
					EnemySquadManagers[EnemySquadNumber]->TryStartTimerToFindTargetLocation();
				}			
			}
		}
	}
}

void AMainGM::SetEnemiesTargetLocation(int32 SquadNumber, FVector TargetLocation)
{
	if(EnemyUnitsInSquads.IsValidIndex(SquadNumber))
	{
		float AcceptanceRadius = GetUnitsOccupiedRadius(EnemyUnitsInSquads[SquadNumber])* 3;
		for (AEnemyUnit* Unit : EnemyUnitsInSquads[SquadNumber])
		{
			if (Unit)
			{
				Unit->SetMoveTargetLocation(TargetLocation, AcceptanceRadius);
			}
		}
	}
}

void AMainGM::SetSelectedAlliesTargetLocation(FVector TargetLocation)
{
	float AcceptanceRadius = GetUnitsOccupiedRadius(SelectedAllyUnits)* 3;
	for (AAllyUnit* Unit : SelectedAllyUnits)
	{
		if(Unit)
		{
			Unit->SetMoveTargetLocation(TargetLocation, AcceptanceRadius);

			if (UnitTargetLocationNiagaraFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), UnitTargetLocationNiagaraFX, TargetLocation, FRotator(90, 0, 0), FVector(1), true, true);
			}
		}
	}
}

void AMainGM::SetAlliesAttackState(bool bIsAttack)
{
	for (AAllyUnit* Unit : SelectedAllyUnits)
	{
		Unit->SetAttackCommand(bIsAttack);
	}
}

void AMainGM::SelectAllyUnit(AAllyUnit* Unit)
{
	if(Unit)
	{
		SelectedAllyUnits.Add(Unit);
		if(SelectAllyUnitNiagaraFX)
		{
			Unit->SelectedUnitFX = UNiagaraFunctionLibrary::SpawnSystemAttached(SelectAllyUnitNiagaraFX, Unit->GetMesh(), NAME_None, FVector(), FRotator(90, 0, 0), EAttachLocation::SnapToTarget, true, true);
		}
	}
}

void AMainGM::UnselectAllyUnit(AAllyUnit* Unit)
{
	if (Unit)
	{
		SelectedAllyUnits.Remove(Unit);
		Unit->SelectedUnitFX->Deactivate();
	}
}

void AMainGM::UnselectAllAllyUnits()
{
	for (AAllyUnit* AllyUnit : SelectedAllyUnits)
	{
		AllyUnit->SelectedUnitFX->Deactivate();
	}
	SelectedAllyUnits.Empty();
}

void AMainGM::SetEnemyToAttackForSelectedAllies(AActor* Actor)
{
	if (Actor)
	{
		//SetAlliesAttackState(true);
		for (AAllyUnit* Unit : SelectedAllyUnits)
		{
			if(Unit)
			{
				//Unit->SetEnemyToAttack(Actor);
				Unit->AttackEnemy(Actor);
			}
		}
		if (AttackEnemyUnitNiagaraFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(AttackEnemyUnitNiagaraFX, Actor->GetRootComponent(), NAME_None, FVector(), FRotator(90, 0, 0), EAttachLocation::SnapToTarget, true, true);
		}
	}
}

void AMainGM::OnEnemyDied(AEnemyUnit* Unit)
{
	if (Unit)
	{			
		EarnedPoints += Unit->PointsForKill;		
		
		if(EnemyUnitsInSquads.IsValidIndex(Unit->SquadNumber))
		{
			EnemyUnitsInSquads[Unit->SquadNumber].Remove(Unit);
		}
		//Each ally that attacked this unit tries find new enemy to attack
		for (AAllyUnit* AllyUnit : AllyUnits)
		{
			if (AllyUnit->GetEnemyToAttack())
			{
				if (AllyUnit->GetEnemyToAttack() == Unit)
				{
					if (!AllyUnit->TryFindNewEnemyToAttack())
					{
						AllyUnit->SetAttackCommand(false);
					}
				}
			}
		}
		if (EnemyUnitsInSquads[Unit->SquadNumber].Num() == 0)
		{
			if (EnemySquadManagers.IsValidIndex(Unit->SquadNumber))
			{
				//Remove squad manager if it was the last unit in the squad
				EnemySquadManagers.RemoveAt(Unit->SquadNumber);
			}
			for(int32 i = Unit->SquadNumber; i < EnemyUnitsInSquads.Num(); i++)
			{
				for (int32 j = 0; j < EnemyUnitsInSquads[i].Num(); j++)
				{
					EnemyUnitsInSquads[i][j]->SquadNumber--;
				}
			}
			EnemyUnitsInSquads.RemoveAt(Unit->SquadNumber);
			//if it was the last enemy squad it'll be the victory
			if (EnemyUnitsInSquads.Num() == 0)
			{
				EndGame(true);
			}
		}
	}
}

void AMainGM::OnAllyUnitDied(AAllyUnit* Unit)
{
	if (Unit)
	{
		//Each enemy that attacked this unit tries find new enemy to attack
		for (TArray<AEnemyUnit*> EnemyUnitSquad : EnemyUnitsInSquads)
		{
			for(AEnemyUnit* EnemyUnit : EnemyUnitSquad)
			{
				if (EnemyUnit->GetEnemyToAttack())
				{
					if (EnemyUnit->GetEnemyToAttack() == Unit)
					{
						EnemyUnit->TryFindNewEnemyToAttack();
					}
				}
			}
		}
		//Removing unit from all arrays
		if (SelectedAllyUnits.Contains(Unit))
		{
			SelectedAllyUnits.Remove(Unit);
		}
		if(AllyUnits.Contains(Unit))
		{
			AllyUnits.Remove(Unit);
		}
		//if it was the last enemy squad it'll be the defeat
		if (AllyUnits.Num() == 0)
		{
			EndGame(false);
		}
	}
}

void AMainGM::ResetEnemyFindTargetTimer(int32 SquadNumber)
{
	if (EnemySquadManagers.IsValidIndex(SquadNumber) && EnemySquadManagers[SquadNumber])
	{
		EnemySquadManagers[SquadNumber]->InvalidateTimerToFindTarget();
	}
}

int32 AMainGM::SaveEarnedPoints()
{
	if (URTSGameInstance* GI = Cast<URTSGameInstance>(GetGameInstance()))
	{
		GI->AddPoints(EarnedPoints);
	}
	return EarnedPoints;
}

void AMainGM::EndGame(bool bIsWin)
{
	if (bIsWin)
	{
		if (URTSGameInstance* GI = Cast<URTSGameInstance>(GetGameInstance()))
		{
			GI->IncrementLevel();
		}
	}
	EndGameBP(bIsWin);
}

void AMainGM::EndGameBP_Implementation(bool bIsWin)
{
	//BP
}
