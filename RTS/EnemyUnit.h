// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "EnemySquadManager.h"
//
#include "EnemyUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API AEnemyUnit : public AUnit
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

	
public:
	UPROPERTY(BlueprintReadOnly)
	int32 SquadNumber = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 PointsForKill = 10;
	UPROPERTY(EditDefaultsOnly)
	float SpawnChance = 0.5f;

	AEnemyUnit();
	virtual bool OnTargetPerceptionUpdated(AActor* Actor) override;
	virtual void TryAttack() override;
	virtual void SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius) override;
	virtual void Death() override;
	virtual void SetEnemyToAttack(AActor* Actor) override;
	virtual void OnAIMoveCompleted() override;
};
