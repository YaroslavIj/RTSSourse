
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnitAIController.h"
#include "Components/WidgetComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Data.h"
//
#include "Unit.generated.h"

UCLASS()
class RTS_API AUnit : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AUnit();

protected:
	virtual void BeginPlay() override;

	//UWidgetComponent* WidgetComponent

	AUnitAIController* UnitAIController = nullptr;
	FVector MoveTargetLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRate = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TArray<UAnimMontage*> AttackAnims;

	FTimerHandle AttackTimer;
	FTimerHandle PerceptionTimer;

	UPROPERTY(BlueprintReadOnly)
	float DefaultWalkSpeed = 500.f;

	UPROPERTY(BlueprintReadOnly)
	AActor* EnemyToAttack = nullptr;

	bool bIsRotatedTowardsTheEnemy = false;
	bool bIsAttackReloading = false;
	bool bIsAttacking = false;

	float Health = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CoefDamage = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bCanDamageTeammates = false;
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* AttackHitFX = nullptr;
	UPROPERTY(EditDefaultsOnly)
	USoundBase* AttackHitSound = nullptr;
	UPROPERTY(EditDefaultsOnly)
	USoundBase* AttackSound = nullptr;
	UPROPERTY(EditDefaultsOnly)
	float AttackDamage = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Controller")
	float AISightRadius = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Controller")
	float AILoseSightRadius = 2000.f;
	UPROPERTY(EditDefaultsOnly)
	bool bSplashDamage = false;
	bool bIsRunningToLocation = false;

	UPROPERTY(BlueprintReadOnly)
	FGenericTeamId GenericTeamId;


public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetMoveTargetLocation(FVector NewTargetLocation, float AcceptanceRadius);

	virtual void Init(FUnitInfo InUnitInfo);

	FORCEINLINE AActor* GetEnemyToAttack() { return EnemyToAttack; }
	FORCEINLINE bool GetIsAlive() { return Health > 0; }
	UFUNCTION()
	virtual bool OnTargetPerceptionUpdated(AActor* Actor);

	//void SetStateAttack(bool InbIsAttack);
	virtual void TryAttack();
	virtual void Attack();

	UFUNCTION()
	virtual void OnMontageNotifyCalled(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterapted);
	UFUNCTION()
	void ResetIsAttackReloading();

	virtual void OnTargetPerceptionForgotten(AActor* Actor);
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	virtual void Death();
	virtual void SetEnemyToAttack(AActor* Actor);
	bool TryFindNewEnemyToAttack();
	virtual void OnAIMoveCompleted();
	void PerceptionTick();
	void MoveToActor(AActor* Actor);
	virtual bool CheckCanAttackUnit(AUnit* Unit);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const override;
	void SetGenericTeamId(int32 NewGenericTeamId);
};
