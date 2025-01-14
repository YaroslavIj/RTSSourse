// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NiagaraComponent.h"
//
#include "AllyUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API AAllyUnit : public AUnit
{
	GENERATED_BODY()
	

protected:

	virtual void BeginPlay() override;
	
	bool bHasAttackCommand = false;
	bool bIsOnTheWay = false;
	UPROPERTY(BlueprintReadOnly)
	int32 EnemySquadToAttack = -1;
public:

	AAllyUnit();

	
	UNiagaraComponent* SelectedUnitFX = nullptr;

	virtual bool OnTargetPerceptionUpdated(AActor* Actor) override;
	virtual void TryAttack() override;
	virtual void SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius) override;
	virtual void OnTargetPerceptionForgotten(AActor* Actor) override;
	virtual void SetEnemyToAttack(AActor* Actor) override;
	virtual void Death() override;
	virtual void OnAIMoveCompleted() override;
	FORCEINLINE void SetAttackCommand(bool bIsAttack) { bHasAttackCommand = bIsAttack; }
	void AttackEnemy(AActor* Actor);
	virtual bool CheckCanAttackUnit(AUnit* Unit) override;
};
