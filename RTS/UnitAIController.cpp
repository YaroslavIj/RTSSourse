// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAIController.h"
#include "Unit.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/CrowdFollowingComponent.h"

AUnitAIController::AUnitAIController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SetPerceptionComponent(*Perception);
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AUnitAIController::OnTargetPerceptionUpdated);
	Perception->OnTargetPerceptionForgotten.AddDynamic(this, &AUnitAIController::OnTargetPerceptionForgotten);
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AUnitAIController::OnAIMoveCompleted);
}

void AUnitAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (AUnit* Unit = Cast<AUnit>(GetPawn()))
	{
		Unit->OnTargetPerceptionUpdated(Actor);
	}
}

void AUnitAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	if (AUnit* Unit = Cast<AUnit>(GetPawn()))
	{
		Unit->OnTargetPerceptionForgotten(Actor);
	}
}

void AUnitAIController::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (AUnit* Unit = Cast<AUnit>(GetPawn()))
	{
		Unit->OnAIMoveCompleted();
	}
}

FGenericTeamId AUnitAIController::GetGenericTeamId() const
{
	return GenericTeamId;
}

void AUnitAIController::SetGenericTeamId(int32 NewGenericTeamId)
{
	GenericTeamId = FGenericTeamId(NewGenericTeamId);
}
