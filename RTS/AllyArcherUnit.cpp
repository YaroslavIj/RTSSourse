// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyArcherUnit.h"

AAllyArcherUnit::AAllyArcherUnit()
{

}

void AAllyArcherUnit::Init(FUnitInfo InUnitInfo)
{
	Super::Init(InUnitInfo);
	ProjectileParams.ProjectileDamage = InUnitInfo.Damage;
}

void AAllyArcherUnit::BeginPlay()
{
	Super::BeginPlay();

	if (AttackHitFX)
	{
		ProjectileParams.HitFX = AttackHitFX;
	}
	if (AttackHitSound)
	{
		ProjectileParams.HitSound = AttackHitSound;
	}
}

void AAllyArcherUnit::Attack()
{
	Super::Attack();
	if (ProjectileClass)
	{
		FVector ProjectileSpawnLocation = GetMesh()->GetSocketLocation(ShootSocketName);
		FRotator ProjectileSpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Owner = this;
		AProjectile* NewProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation, SpawnParams);
		NewProjectile->InitProjectile(ProjectileParams, true, bCanDamageTeammates);
	}
}
