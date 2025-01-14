// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyUnit.h"
#include "MeleeEnemyUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API AMeleeEnemyUnit : public AEnemyUnit
{
	GENERATED_BODY()

public:
	virtual void Attack() override;

	UPROPERTY(EditDefaultsOnly)
	float AttackWidth = 50.f;
	UPROPERTY(EditDefaultsOnly)
	float AttackLength = 120.f;
	UPROPERTY(EditDefaultsOnly)
	float AttackStartLength = 60.f;
};
