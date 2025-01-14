// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyUnit.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "EnemyUnit.h"
#include "MainGM.h"

AAllyUnit::AAllyUnit()
{
	SetGenericTeamId(0);
}

void AAllyUnit::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	}
	if(UnitAIController)
	{
		//UnitAIController->GetPathFollowingComponent
	}
}

bool AAllyUnit::OnTargetPerceptionUpdated(AActor* Actor)
{
	AActor* LastEnemyToAttack = EnemyToAttack;
	bool bIsDanger = false;
	if (!bHasAttackCommand)
	{
		bIsDanger = Super::OnTargetPerceptionUpdated(Actor);
	}
	else if(!EnemyToAttack)
	{
		if(Actor->GetClass()->IsChildOf(AEnemyUnit::StaticClass()))
		{
			if (AEnemyUnit* EnemyUnit = Cast<AEnemyUnit>(Actor))
			{
				if (EnemyUnit->SquadNumber == EnemySquadToAttack)
				{
					SetEnemyToAttack(Actor);
					//bIsOnTheWay = false;
				}
			}
		}
	}
	if(bIsDanger && bHasAttackCommand)
	{
		if (LastEnemyToAttack != EnemyToAttack)
		{
			UnitAIController->MoveToActor(EnemyToAttack, -1.f, true);
		}
	}

	return bIsDanger;
}

void AAllyUnit::TryAttack()
{
	if (!bHasAttackCommand)
	{
		float CurrentSpeed = GetVelocity().Length();
		if (FMath::IsNearlyZero(CurrentSpeed, 0.1f))
		{
			Super::TryAttack();
		}
	}
	else
	{
		Super::TryAttack();
	}
}
void AAllyUnit::SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius)
{
	Super::SetMoveTargetLocation(NewTargetLocation, AcceptanceRadius);

	bHasAttackCommand = false;
	EnemyToAttack = nullptr;
}

void AAllyUnit::OnTargetPerceptionForgotten(AActor* Actor)
{
	if (!bHasAttackCommand)
	{
		Super::OnTargetPerceptionForgotten(Actor);
	}
}

void AAllyUnit::SetEnemyToAttack(AActor* Actor)
{
	//Super::SetEnemyToAttack(Actor);
	EnemyToAttack = Actor;
	if (Actor)
	{
		if (bHasAttackCommand)
		{
			UnitAIController->MoveToActor(Actor, 1, true);
		}
	}

}

void AAllyUnit::Death()
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (AMainGM* GM = Cast<AMainGM>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnAllyUnitDied(this);
		}
	}

	Super::Death();
}

void AAllyUnit::OnAIMoveCompleted()
{
	TryFindNewEnemyToAttack();
}

void AAllyUnit::AttackEnemy(AActor* Actor)
{
	if(AEnemyUnit* Enemy = Cast<AEnemyUnit>(Actor))
	{		
		//bIsOnTheWay = true;
		bHasAttackCommand = true;
		EnemySquadToAttack = Enemy->SquadNumber;
		if (!TryFindNewEnemyToAttack())
		{
			UnitAIController->MoveToActor(Enemy, 1, true);
		}
	}
}

bool AAllyUnit::CheckCanAttackUnit(AUnit* Unit)
{
	if (AEnemyUnit* Enemy = Cast<AEnemyUnit>(Unit))
	{
		if(bHasAttackCommand)
		{
			if (Enemy->SquadNumber == EnemySquadToAttack)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}
