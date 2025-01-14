// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUnit.h"
#include "Perception/AIPerceptionComponent.h"
#include "MainGM.h"
#include "GameFramework/CharacterMovementComponent.h"

void AEnemyUnit::BeginPlay()
{
	Super::BeginPlay();
	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	}
}

AEnemyUnit::AEnemyUnit()
{
	SetGenericTeamId(1);
}

bool AEnemyUnit::OnTargetPerceptionUpdated(AActor* Actor)
{
	AActor* LastEnemyToAttack = EnemyToAttack;
	bool bIsDanger = Super::OnTargetPerceptionUpdated(Actor);
	if (bIsDanger)
	{
		//if(LastEnemyToAttack != EnemyToAttack)
		//{
		//	//UnitAIController->MoveToActor(EnemyToAttack, -1.f, true);
		//}
	}
	return bIsDanger;
}

void AEnemyUnit::TryAttack()
{
	Super::TryAttack();

}

void AEnemyUnit::SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius)
{
	if (!EnemyToAttack)
	{
		Super::SetMoveTargetLocation(NewTargetLocation, AcceptanceRadius);
	}
}

void AEnemyUnit::Death()
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnEnemyDied(this);
		}
	}
	Super::Death();
}

void AEnemyUnit::SetEnemyToAttack(AActor* Actor)
{
	AActor* LastEnemyToAttack = EnemyToAttack;
	Super::SetEnemyToAttack(Actor);
	if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
	{	
		if (Actor)
		{
			UnitAIController->MoveToActor(Actor, 1, true);

			if (LastEnemyToAttack != Actor)
			{
				GM->SetEnemiesTargetLocation(SquadNumber, Actor->GetActorLocation());
			}
			GM->ResetEnemyFindTargetTimer(SquadNumber);
		}
		else
		{
			if (UEnemySquadManager* Manager = GM->GetEnemySquadManager(SquadNumber))
			{
				Manager->TryStartTimerToFindTargetLocation();
			}
		}
	}
}

void AEnemyUnit::OnAIMoveCompleted()
{
	if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
	{
		if (UEnemySquadManager* Manager = GM->GetEnemySquadManager(SquadNumber))
		{
			Manager->TryStartTimerToFindTargetLocation();
		}
	}
}
