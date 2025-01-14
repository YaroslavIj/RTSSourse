// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
//
#include "EnemySquadManager.generated.h"

/**
 * 
 */
class AMainGM;
class AEnemyUnit;

UCLASS()
class RTS_API UEnemySquadManager : public UObject
{
	GENERATED_BODY()

protected:

	int32 SquadNumber = -1;
	UPROPERTY()
	AMainGM* GM = nullptr;
	FTimerHandle FindTargetTimer;
	float FindTargetRate = 5.f;
	float FindTargetRadius = 2000.f;
	TArray<AEnemyUnit*> UnitsToManage;
public:

	void Init(int32 InSquadNumber, AMainGM* InGM, float InFindTargetRate, float InFindTargetRadius);
	void FindRandomTargetLocation();
	void InvalidateTimerToFindTarget();
	void TryStartTimerToFindTargetLocation();
};
