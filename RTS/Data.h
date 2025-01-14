// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data.generated.h"

class AUnit;

UCLASS()
class RTS_API UData : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

};

USTRUCT(BlueprintType)
struct FUnitInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AUnit> UnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UnitName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRate = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoefDamage = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ParameterUpdatePrice = 50;
};
