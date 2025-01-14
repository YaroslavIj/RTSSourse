
#include "Unit.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "NavigationSystem.h"

AUnit::AUnit()
{
 	
	PrimaryActorTick.bCanEverTick = true;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &AUnit::ReceiveDamage);

	if(AIControllerClass && AIControllerClass->IsChildOf(AUnitAIController::StaticClass()))
	{
		
		UnitAIController = GetWorld()->SpawnActor<AUnitAIController>(AIControllerClass);
		UnitAIController->Possess(this);
		//UnitAIController->SetGenericTeamId(TeamId);
		FAISenseID SenseID = UAISense::GetSenseID<UAISense_Sight>();

		if (UAISenseConfig_Sight* ConfigSight = Cast<UAISenseConfig_Sight>(UnitAIController->Perception->GetSenseConfig(SenseID)))
		{
			ConfigSight->SightRadius = AISightRadius;
			ConfigSight->LoseSightRadius = AILoseSightRadius;

			/*ConfigSight->DetectionByAffiliation.bDetectEnemies = true;
			ConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
			ConfigSight->DetectionByAffiliation.bDetectNeutrals = false;*/
		}
	}
	//UnitAIController->
	if(GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AUnit::OnMontageNotifyCalled);
	}
	//if(GetCharacterMovement())
	//{
	//	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(PerceptionTimer, this, &AUnit::PerceptionTick, 0.5f, true);
	}
}

void AUnit::Init(FUnitInfo InUnitInfo)
{
	AttackDamage = InUnitInfo.Damage;
	AttackRate = InUnitInfo.AttackRate;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = InUnitInfo.MovementSpeed;
		DefaultWalkSpeed = InUnitInfo.MovementSpeed;
	}
	CoefDamage = InUnitInfo.CoefDamage;
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Health > 0)
	{
		if (EnemyToAttack)
		{
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyToAttack->GetActorLocation());
			if (!FMath::IsNearlyEqual(GetActorRotation().Yaw, TargetRotation.Yaw, 0.1))
			{
				if (FMath::IsNearlyZero(GetVelocity().Length(), 0.1f))
				{
					FRotator NeedToRotate = UKismetMathLibrary::ComposeRotators(TargetRotation, GetActorRotation() * -1.f);
					float RotationRate = GetCharacterMovement()->RotationRate.Yaw;
					float RotationRateDelta = RotationRate * DeltaTime;
					if (NeedToRotate.Yaw < 0)
					{
						RotationRateDelta = -RotationRateDelta;
					}
					if (GetActorRotation().Yaw + RotationRateDelta > TargetRotation.Yaw || GetActorRotation().Yaw + RotationRateDelta < TargetRotation.Yaw)
					{
						SetActorRotation(FRotator(GetActorRotation().Pitch, TargetRotation.Yaw, GetActorRotation().Roll));
						bIsRotatedTowardsTheEnemy = true;
					}
					else
					{
						SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + RotationRateDelta, GetActorRotation().Roll));
						bIsRotatedTowardsTheEnemy = false;
					}
				}
			}
			else
			{
				bIsRotatedTowardsTheEnemy = true;
			}
			float DistanceToEnemy = FVector::Distance(EnemyToAttack->GetActorLocation(), GetActorLocation());
			if (DistanceToEnemy <= AttackRange)
			{
				TryAttack();
			}
			else
			{
				if (GetCharacterMovement()->MaxWalkSpeed == 0)
				{
					GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
				}
			}
		}
	}
}

void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AUnit::SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		FNavLocation OutNavLocation;
		NavSystem->GetRandomReachablePointInRadius(NewTargetLocation, AcceptanceRadius, OutNavLocation);
		FVector MoveLocation = OutNavLocation.Location;
		bIsRunningToLocation = true;
		MoveTargetLocation = MoveLocation;
		UnitAIController->MoveToLocation(MoveLocation, 100, true, true);
	}

}

bool AUnit::OnTargetPerceptionUpdated(AActor* Actor)
{
	if(Actor)
	{
		if (Actor->GetClass()->IsChildOf(AUnit::StaticClass()))
		{
			if(Actor->GetClass()->IsChildOf(AEnemyUnit::StaticClass()) && this->GetClass()->IsChildOf(AAllyUnit::StaticClass()) ||
			   this->GetClass()->IsChildOf(AEnemyUnit::StaticClass()) && Actor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
			{
				float DistanceToEnemy = FVector::Distance(Actor->GetActorLocation(), GetActorLocation());					
				if (EnemyToAttack)
				{
					float LastDistanceToEnemy = FVector::Distance(EnemyToAttack->GetActorLocation(), GetActorLocation());
					if (DistanceToEnemy < LastDistanceToEnemy)
					{
						SetEnemyToAttack(Actor);
					}
				}
				else
				{
					SetEnemyToAttack(Actor);
				}
				return true;					
			}
		}
	}
	return false;
}

//void AUnit::SetStateAttack(bool InbIsAttack)
//{
//	if(InbIsAttack != bShouldAttack)
//	{
//		bShouldAttack = InbIsAttack;
//		if (GetWorld())
//		{
//			if (InbIsAttack && !bShouldAttack && bIsRotatedTowardsTheEnemy)
//			{
//				bShouldAttack = true;
//			}
//			else if (AttackTimer.IsValid())
//			{
//				GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
//				bShouldAttack = false;
//			}
//		}
//	}
//}

void AUnit::TryAttack()
{	
	GetCharacterMovement()->MaxWalkSpeed = 0;
	if (bIsRotatedTowardsTheEnemy)
	{
		if (!bIsAttacking && !bIsAttackReloading)
		{
			if (GetMesh() && GetMesh()->GetAnimInstance())
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if(AnimInstance)
				{
					//int32 RandomAnimIndex = UKismetMathLibrary::RandomInteger(AttackAnims.Num() - 1);
					int32 RandomAnimIndex = FMath::RandRange(0, AttackAnims.Num() - 1);
					if(AttackAnims.IsValidIndex(RandomAnimIndex) && AttackAnims[RandomAnimIndex])
					{
						AnimInstance->Montage_Play(AttackAnims[RandomAnimIndex]);
						FAnimMontageInstance* Montage = AnimInstance->GetActiveMontageInstance();
						Montage->OnMontageEnded.BindUObject(this, &AUnit::OnAttackMontageEnded);
						bIsAttacking = true;
					}
				}
			}
		}
	}
}

void AUnit::OnMontageNotifyCalled(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if(NotifyName == FName("Attack"))
	{
		Attack();
	}
}

void AUnit::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterapted)
{
	bIsAttackReloading = true;
	bIsAttacking = false;
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AUnit::ResetIsAttackReloading, AttackRate, false);
	}
}

void AUnit::ResetIsAttackReloading()
{	
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	bIsAttackReloading = false;
}

void AUnit::OnTargetPerceptionForgotten(AActor* Actor)
{
	if (EnemyToAttack == Actor)
	{
		EnemyToAttack = nullptr;
	}
}

void AUnit::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	float DamageWithCoef = Damage * CoefDamage;
	if(Health > 0)
	{
		if (Health - DamageWithCoef <= 0)
		{
			Health = 0;
			Death();
		}
		else
		{
			Health -= DamageWithCoef;
		}
	}
}

void AUnit::Death()
{
	if (UnitAIController)
	{
		UnitAIController->UnPossess();
		UnitAIController->Destroy();
	}
	//if (GetMesh() && GetCapsuleComponent())
	//{
	//	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	//	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	//	GetMesh()->SetSimulatePhysics(true);
	//}
	Destroy();
}

void AUnit::SetEnemyToAttack(AActor* Actor)
{
	EnemyToAttack = Actor;
}

bool AUnit::TryFindNewEnemyToAttack()
{
	TArray<AActor*> PerceivedActors;
	if(IsValid(UnitAIController) && IsValid(UnitAIController->Perception) && UAISense_Sight::StaticClass())
	{
		UnitAIController->Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
		AActor* NewActorToAttack = nullptr;
		float DistanceToNearestActor = 0.f;
		if (PerceivedActors.Num() > 0)
		{
			for (AActor* PerceivedActor : PerceivedActors)
			{
				if (this->GetClass()->IsChildOf(AAllyUnit::StaticClass()) && PerceivedActor->GetClass()->IsChildOf(AEnemyUnit::StaticClass()) ||
					this->GetClass()->IsChildOf(AEnemyUnit::StaticClass()) && PerceivedActor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
				{
					if (AUnit* Enemy = Cast<AUnit>(PerceivedActor))
					{
						if (Enemy->GetIsAlive())
						{
							if(CheckCanAttackUnit(Enemy))
							{
								if (!NewActorToAttack)
								{
									NewActorToAttack = PerceivedActor;
									DistanceToNearestActor = FVector::Distance(GetActorLocation(), PerceivedActor->GetActorLocation());
								}
								else
								{
									float NewDistance = FVector::Distance(GetActorLocation(), PerceivedActor->GetActorLocation());
									if (NewDistance < DistanceToNearestActor)
									{
										NewActorToAttack = PerceivedActor;
										DistanceToNearestActor = NewDistance;
									}
								}
							}
						}
					}
				}
			}
		}
		SetEnemyToAttack(NewActorToAttack);
		if (!NewActorToAttack)
		{
			UnitAIController->StopMovement();
		}
		if(NewActorToAttack)
		{
			return true;
		}		
	}
	return false;
}

void AUnit::OnAIMoveCompleted()
{

}

void AUnit::PerceptionTick()
{
	if (EnemyToAttack)
	{
		TryFindNewEnemyToAttack();
	}
}

void AUnit::MoveToActor(AActor* Actor)
{
	if(Actor && UnitAIController)
	{
		UnitAIController->MoveToActor(Actor, 1, true);
	}
}

bool AUnit::CheckCanAttackUnit(AUnit* Unit)
{
	// Function for ally child unit
	return true;
}

void AUnit::Attack()
{
	if(AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
	}
}
FGenericTeamId AUnit::GetGenericTeamId() const
{
	return GenericTeamId;
}

void AUnit::SetGenericTeamId(int32 NewGenericTeamId)
{
	GenericTeamId = FGenericTeamId(NewGenericTeamId);
}
