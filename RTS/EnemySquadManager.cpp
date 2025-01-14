// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySquadManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "EnemyUnit.h"
#include "MainGM.h"

void UEnemySquadManager::Init(int32 InSquadNumber, AMainGM* InGM, float InFindTargetRate, float InFindTargetRadius)
{
	FindTargetRadius = InFindTargetRadius;
	FindTargetRate = InFindTargetRate;
	SquadNumber = InSquadNumber;
	if(InGM)
	{
		GM = InGM;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemySquadManager::Init - GM is invalid"));
	}
	//if (GM && GM->GetWorld())
	//{
	//	GM->GetWorld()->GetTimerManager().SetTimer(FindTargetTimer, this, &UEnemySquadManager::FindRandomTargetLocation, FindTargetRate, true);
	//}
	//for (AEnemyUnit* Unit : GM->GetEnemyUnitsInSquads()[InSquadNumber])
	//{
	//	if (Unit)
	//	{
	//		UnitsToManage.Add(Unit);
	//	}
	//}
}

void UEnemySquadManager::FindRandomTargetLocation()
{
	if(GM)
	{
		if(GM->GetEnemyUnitsInSquads().IsValidIndex(SquadNumber))
		{
			TArray<AEnemyUnit*> Units = GM->GetEnemyUnitsInSquads()[SquadNumber];
			//int32 RandomIndex = UKismetMathLibrary::RandomInteger(Units.Num() - 1);
			int32 RandomIndex = FMath::RandRange(0, Units.Num() - 1);
			UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GM->GetWorld());
			if (NavSystem)
			{
				if (Units.IsValidIndex(RandomIndex) && Units[RandomIndex])
				{
					FVector StartLocation = Units[RandomIndex]->GetActorLocation();
					FNavLocation OutNavLocation;
					if (NavSystem->GetRandomReachablePointInRadius(StartLocation, FindTargetRadius, OutNavLocation))
					{
						FVector OutLocation = OutNavLocation.Location;
						GM->SetEnemiesTargetLocation(SquadNumber, OutLocation);
					}
				}
			}
		}
	}
}

void UEnemySquadManager::InvalidateTimerToFindTarget()
{
	if (FindTargetTimer.IsValid())
	{
		FindTargetTimer.Invalidate();
	}
}

void UEnemySquadManager::TryStartTimerToFindTargetLocation()
{
	bool bNeedToFindNewLocation = true;
	if (GM)
	{
		TArray<TArray<AEnemyUnit*>> EnemyUnitsIsSquads = GM->GetEnemyUnitsInSquads();
		if(EnemyUnitsIsSquads.Num() > 0)
		{
			if (EnemyUnitsIsSquads.IsValidIndex(SquadNumber))
			{
				TArray<AEnemyUnit*> Units = EnemyUnitsIsSquads[SquadNumber];
				for (AEnemyUnit* Unit : Units)
				{
					if (Unit)
					{
						if (Unit->GetEnemyToAttack() != nullptr)
						{
							bNeedToFindNewLocation = false;
						}
					}
				}
			}
		}
		if (bNeedToFindNewLocation)
		{
			if(GM->GetWorld())
			{
				GM->GetWorld()->GetTimerManager().SetTimer(FindTargetTimer, this, &UEnemySquadManager::FindRandomTargetLocation, FindTargetRate, false);
			}
		}
	}
}
