// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
//
#include "UnitAIController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API AUnitAIController : public AAIController
{
	GENERATED_BODY()
	
public: 

	UPROPERTY(BlueprintReadOnly)
	FGenericTeamId GenericTeamId;

	AUnitAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception")
	UAIPerceptionComponent* Perception = nullptr;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	void OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);



	//GenericTeamId
	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const override;
	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(int32 NewGenericTeamId);
};
