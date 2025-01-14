// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "EnemyUnit.h"
#include "AllyUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MainGM.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	//SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	//RootComponent = SceneComponent;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//ProjectileMesh->SetupAttachment(RootComponent);
	RootComponent = ProjectileMesh;
	ProjectileMesh->SetCollisionProfileName(FName("Projectile"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::InitProjectile(FProjectileParams Params, bool InbIsAllyProjectile, bool InbCanDamageTeammates)
{
	if (GetOwner())
	{
		ProjectileMesh->IgnoreActorWhenMoving(GetOwner(), true);
		if(!bCanDamageTeammates)
		{
			if (InbIsAllyProjectile)
			{
				ProjectileMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
			}
			else
			{
				ProjectileMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
			}
		}
	}
	bIsAllyProjectile = InbIsAllyProjectile;
	bCanDamageTeammates = InbCanDamageTeammates;
	if (Params.ProjectileMesh && ProjectileMesh)
	{
		ProjectileMesh->SetStaticMesh(Params.ProjectileMesh);
	}
	if (ProjectileMovement)
	{
		ProjectileMovement->MaxSpeed = Params.ProjectileSpeed;
		FVector Direction = GetActorForwardVector();
		ProjectileMovement->Velocity = Direction* Params.ProjectileSpeed;
	}
	Damage = Params.ProjectileDamage;
	if(Params.HitFX)
	{
		HitFX = Params.HitFX;
	}
	if(Params.HitSound)
	{
		HitSound = Params.HitSound;
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		if (OtherActor->GetClass()->IsChildOf(AUnit::StaticClass()))
		{
			if (GetInstigator() && GetInstigator()->GetController())
			{
				if (!bCanDamageTeammates)
				{
					if (bIsAllyProjectile != OtherActor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
					{
						UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, nullptr);		
					}
				}
				else
				{
					UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, nullptr);
				}
			}
		}
	}
	if(GetWorld())
	{
		if (HitFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitFX, GetActorTransform());
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}
	}
	Destroy();
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->GetClass()->IsChildOf(AUnit::StaticClass()))
		{
			if (GetInstigator() && GetInstigator()->GetController())
			{
				if (!bCanDamageTeammates)
				{
					if (bIsAllyProjectile != OtherActor->GetClass()->IsChildOf(AAllyUnit::StaticClass()))
					{
						UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, nullptr);
						Destroy();
					}
				}
				else
				{
					UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, nullptr);
					Destroy();
				}
			}
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
