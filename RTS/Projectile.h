// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
//
#include "Projectile.generated.h"

USTRUCT(BlueprintType)
struct FProjectileParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float ProjectileSpeed = 1000.f;
	float ProjectileDamage = 10.f;
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* ProjectileMesh = nullptr;
	UParticleSystem* HitFX = nullptr;
	USoundBase* HitSound = nullptr;
};

UCLASS()
class RTS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement = nullptr;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scene")
	//USceneComponent* SceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* ProjectileMesh = nullptr;
	bool bIsAllyProjectile;
	bool bCanDamageTeammates = false;
	float Damage = 0.f;
	UParticleSystem* HitFX = nullptr;
	USoundBase* HitSound = nullptr;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitProjectile(FProjectileParams Params, bool InbIsAllyProjectile, bool InbCanDamageTeammates);
};
