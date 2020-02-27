// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlockEmitter.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

#define MAX_FLOCK_HEALTH_BASE 10.0f
#define DISABLED 0

UENUM()
enum class EEnemyBehavaiour : uint32
{
	eNavigate = 0x01,
	eShoot = 0x02,
	eFormCircle = 0x04,
};

// macro to override operators for bit-wise operations
ENUM_CLASS_FLAGS(EEnemyBehavaiour)

USTRUCT()
struct FWeights
{
	GENERATED_USTRUCT_BODY()

	float Chase;
	float Seperate;
	float Cohere;
	float Align;
	float Wander;
	float Scutter;
	float Shape;

	FWeights() :
		Chase(DISABLED),
		Seperate(DISABLED),
		Cohere(DISABLED),
		Align(DISABLED),
		Wander(DISABLED),
		Scutter(DISABLED),
		Shape(DISABLED)
	{}

	FWeights(float InChase, float InSeperate, float InCohere, float InAlign, float InWander, float InScutter, float InShape) :
		Chase(InChase),
		Seperate(InSeperate),
		Cohere(InCohere),
		Align(InAlign),
		Wander(InWander),
		Scutter(InScutter),
		Shape(InShape)
	{}
};

USTRUCT()
struct FBoidState
{
	GENERATED_USTRUCT_BODY()
	
	FVector Velocity;
	FVector TotalForce;
	float Health;
	FWeights Weights;
	float RadiusFromTarget;
	float FireCooldown;
	bool bIsFiring;
	bool bIsPlayer;
	UPrimitiveComponent* Target;

	FBoidState() :
		Velocity(FVector::ZeroVector),
		TotalForce(FVector::ZeroVector),
		Health(MAX_FLOCK_HEALTH_BASE),
		RadiusFromTarget(0.0f),
		FireCooldown(0.0f),
		bIsFiring(false),
		bIsPlayer(false),
		Target(nullptr)
	{}
};

UCLASS()
class TWINSTICKSHOOTER_API AFlockEmitter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockEmitter();

	void Init(EEnemyBehavaiour Behaviour, FBoidState InBoidState, int32 NumToSpawn, FLinearColor InColour, float InScale, int32 SpawnRangeRadius);
	
	UPROPERTY()
	TArray<UStaticMeshComponent*> BoidMesh;
	
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* BoidInstanceMesh;

	UPROPERTY()
	TArray <UMaterialInstanceDynamic*> MaterialInstance;

	TArray<FBoidState> BoidState;

	UPROPERTY()
	int32 FlockSize = 100;

	float BoidMass = 0.5f;
	float BaseSpeed = 1000.0f;
	float BaseDrag = .0;

	EEnemyBehavaiour BoidBehaviour;

	void Chase(float DeltaTime, float Radius);
	void Separate(float DeltaTime, float Radius);
	void Cohere(float DeltaTime, float Radius);
	void Align(float DeltaTime);
	void Scutter(float DeltaTime, float Radius);
	void Shoot(float DeltaTime, FVector Direction);
	void FormShape(float DeltaTime, float Radius);
	void View(float DeltaTime);
	FVector GetBoidWorldLocation(UPrimitiveComponent* Boid);
	FVector Steer(const FVector& Dir) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float MaxSpeed = 1.0f;

private:
	const float m_PhysTimeStep = 1.0f / 30.0f;
	float m_PhysTime = 0.0f;
};
