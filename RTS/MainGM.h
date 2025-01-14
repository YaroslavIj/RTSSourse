// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyUnit.h"
#include "AllyUnit.h"
#include "Containers/Array.h"
#include "NiagaraSystem.h"
#include "Data.h"
//
#include "MainGM.generated.h"

class UEnemySquadManager;

UCLASS()
class RTS_API AMainGM : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadOnly)
	TArray<TArray<AEnemyUnit*>> EnemyUnitsInSquads;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AEnemyUnit>> EnemyUnitClasses;
	UPROPERTY(BlueprintReadOnly)
	TArray<AAllyUnit*> AllyUnits;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AAllyUnit>> AllyUnitClasses;

	UPROPERTY(BlueprintReadOnly)
	TArray<AAllyUnit*> SelectedAllyUnits;

	void SpawnUnits(bool bIsAllyUnits, int32 UnitsCount);
	UPROPERTY(EditDefaultsOnly)
	int32 AllyUnitsNumToSpawn = 5;
	UPROPERTY(EditDefaultsOnly)
	int32 EnemyUnitsNumToSpawn = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	UNiagaraSystem* SelectAllyUnitNiagaraFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	UNiagaraSystem* AttackEnemyUnitNiagaraFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	UNiagaraSystem* UnitTargetLocationNiagaraFX = nullptr;

	UPROPERTY()
	TArray<UEnemySquadManager*> EnemySquadManagers;

	UPROPERTY(EditDefaultsOnly, Category = "EnemySquadSettings")
	float EnemySquadFindTargetRate = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySquadSettings")
	float EnemySquadFindTargetRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<FUnitInfo> UnitsInfo;

	UPROPERTY(BlueprintReadWrite)
	int32 EarnedPoints = 0;

	UPROPERTY(EditDefaultsOnly)
	float PercentOfAdditionalEnemyUnitsPerLevel = 0.3;

public:

	FORCEINLINE TArray<TArray<AEnemyUnit*>> GetEnemyUnitsInSquads() { return EnemyUnitsInSquads; }
	FORCEINLINE TArray<AAllyUnit*> GetAllyUnits() { return AllyUnits; }
	FORCEINLINE TArray<AAllyUnit*> GetSelectedAllyUnits() { return SelectedAllyUnits; }
	FORCEINLINE int32 GetEarnedPoints() { return EarnedPoints; }

	UEnemySquadManager* GetEnemySquadManager(int32 SquadIndex);

	UFUNCTION(BlueprintCallable)
	void SpawnAllyUnits(TMap<TSubclassOf<AAllyUnit>, int32> UnitsToSpawn, FVector SpawnLocation);
	UFUNCTION(BlueprintCallable)
	void SpawnEnemyUnits(int32 UnitsCount);

	void SetEnemiesTargetLocation(int32 SquadNumber, FVector TargetLocation);
	UFUNCTION(BlueprintCallable)
	void SetSelectedAlliesTargetLocation(FVector TargetLocation);
	UFUNCTION(BlueprintCallable)
	void SetAlliesAttackState(bool bIsAttack);
	UFUNCTION(BlueprintCallable)
	void SelectAllyUnit(AAllyUnit* Unit);
	UFUNCTION(BlueprintCallable)
	void UnselectAllyUnit(AAllyUnit* Unit);
	UFUNCTION(BlueprintCallable)
	void UnselectAllAllyUnits();
	UFUNCTION(BlueprintCallable)
	void SetEnemyToAttackForSelectedAllies(AActor* Actor);
	void OnEnemyDied(AEnemyUnit* Unit);
	void OnAllyUnitDied(AAllyUnit* Unit);

	template<class T>
	float GetUnitsOccupiedRadius(TArray<T*> Units)
	{
		float AllUnitsSquare = 0;
		for (T* It : Units)
		{
			if(AUnit* Unit = Cast<AUnit>(It))
			{
				float UnitRadius = Unit->GetCapsuleComponent()->GetScaledCapsuleRadius();
				float UnitSquare = powf(UnitRadius, 2) * PI;
				AllUnitsSquare += UnitSquare;
			}
		}
		float UllUnitsRadius = sqrtf(AllUnitsSquare / PI);

		return UllUnitsRadius;
	}

	void ResetEnemyFindTargetTimer(int32 SquadNumber);

	UFUNCTION(BlueprintCallable)
	int32 SaveEarnedPoints();
	//EndGame
	void EndGame(bool bIsWin);
	UFUNCTION(BlueprintNativeEvent)
	void EndGameBP(bool bIsWin);
};
