// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAllyUnit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyUnit.h"

void AMeleeAllyUnit::Attack()
{
	Super::Attack();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<FHitResult> HitResults;
	FVector Start = GetActorLocation() + GetActorForwardVector() * AttackStartLength;
	FVector End = GetActorLocation() + GetActorForwardVector() * AttackLength;
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Start, End, FVector(AttackWidth), GetActorRotation(), ETraceTypeQuery::TraceTypeQuery4, true, IgnoreActors, EDrawDebugTrace::None, HitResults, true);
	for (FHitResult Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->GetClass()->IsChildOf(AEnemyUnit::StaticClass()))
			{
				UGameplayStatics::ApplyDamage(HitActor, AttackDamage, GetController(), this, nullptr);
				if (AttackHitFX)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackHitFX, Hit.ImpactPoint);
				}
				if (AttackHitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackHitSound, Hit.ImpactPoint);
				}
				if (!bSplashDamage)
				{
					break;
				}
			}
		}
	}
}
