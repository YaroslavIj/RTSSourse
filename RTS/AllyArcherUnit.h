// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AllyUnit.h"
#include "Projectile.h"
//
#include "AllyArcherUnit.generated.h"

UCLASS()
class RTS_API AAllyArcherUnit : public AAllyUnit
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;
public:

	virtual void Attack() override;

	AAllyArcherUnit();

	UPROPERTY(EditDefaultsOnly)
	FProjectileParams ProjectileParams;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly)
	FName ShootSocketName = FName("ArrowSocket");

	virtual void Init(FUnitInfo InUnitInfo) override;
};
