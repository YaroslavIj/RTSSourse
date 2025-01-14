// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data.h"
#include "AllyUnit.h"
//
#include "RTSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API URTSGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<AAllyUnit>, int32> AllyUnitsToSpawn;
	UPROPERTY(EditDefaultsOnly)
	TArray<FUnitInfo> AllyUnitsInfo;
	//UDataTable* AllyUnitsInfo = nullptr;
	UPROPERTY(EditDefaultsOnly)
	int32 Points = 0;

	int32 LevelNumber = 0;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TMap<TSubclassOf<AAllyUnit>, int32> GetAllyUnitsToSpawn() { return AllyUnitsToSpawn; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void IncrementLevel() { LevelNumber++; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetLevelNumber() { return LevelNumber; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetAllyUnitsToSpawn(TMap<TSubclassOf<AAllyUnit>, int32> NewAllyUnitsToSpawn) { AllyUnitsToSpawn = NewAllyUnitsToSpawn; }
	UFUNCTION(BlueprintCallable)
	bool GetAllyUnitInfo(TSubclassOf<AAllyUnit> UnitClass, FUnitInfo& OutInfo);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetPoints() { return Points; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AddPoints(int32 SumPoints) { Points += SumPoints; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<FUnitInfo> GetAllAllyUnitsInfo() { return AllyUnitsInfo; }
	UFUNCTION(BlueprintCallable)
	bool RemovePoints(int32 SumPoints);
	UFUNCTION(BlueprintCallable)
	bool UpdateAllyUnitParameter(TSubclassOf<AAllyUnit> UnitClass, FName ParameterName, float UpdateValue, float& OutValue);
};
