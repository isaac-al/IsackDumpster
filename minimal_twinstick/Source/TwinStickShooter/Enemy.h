// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EEnemyType : uint8
{
	ET_Basic 	UMETA(DisplayName = "Basic"),
	ET_Shooter	UMETA(DisplayName = "Shooter"),
};


class ATwinStickShooterProjectile;

UCLASS()
class TWINSTICKSHOOTER_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	EEnemyType EnemyType;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		FVector GunOffset;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ATwinStickShooterProjectile> ProjectileClass;

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
		class UStaticMesh* BasicMesh;

	UPROPERTY(VisibleAnywhere)
		class UStaticMesh* ShooterMesh;

	/* The collision component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphere;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ScutterWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float WanderWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float SeekWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float SeparateWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float AlignWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float CohereWeight = 1.0f;

	UPROPERTY(Category = "Motion", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 400.0f;

	UPROPERTY(Category = "Motion", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxForce = 0.1f;

	UPROPERTY(Category = "Motion", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DesiredForce;

	UPROPERTY(Category = "Motion", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float WanderTheta = 0;

	UPROPERTY()
	float DefaultHealth = 100.0f;

	UPROPERTY()
	float CurrentHealth;

	UPROPERTY()
	TArray<AActor*> OtherEnemyArray;

	// Functions
	/////////////

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual FVector Seek(FVector pTarget);

	UFUNCTION()
	virtual FVector Separate(); 

	UFUNCTION()
	virtual FVector Wander();

	UFUNCTION()
	virtual FVector Scutter(float DeltaTime);

	UFUNCTION()
	virtual FVector Align();

	UFUNCTION()
	virtual FVector Cohere();

	UFUNCTION()
	void AddTotalDesiredForce(FVector pForce);

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection, float DeltaTime);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	UFUNCTION()
	void Init(EEnemyType pType, float pSctr, float pWndr, float pSeek, float pSep, float pAlign, float pCoh);

	virtual FVector Steer(FVector pDir);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser);

	UFUNCTION()
	void DetractHealth(float pAmount);
};
