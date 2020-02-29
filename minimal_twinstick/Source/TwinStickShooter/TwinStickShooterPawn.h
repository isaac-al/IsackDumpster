// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TwinStickShooter.h"
#include "UnrealMathUtility.h"
#include "TwinStickShooterPawn.generated.h"

#define NUM_PLAYER_TYPE 2

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FPlayerDead );

class ATwinStickShooterProjectile;
class USphereComponent;


UCLASS(Blueprintable)
class ATwinStickShooterPawn : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ATwinStickShooterProjectile> ProjectileClass;

	// Forward declaring friend struct instead of using getters and setters
	friend struct Base_PlayerMove;
	friend struct FPS_PlayerMove;
	friend struct God_PlayerMove;

	Base_PlayerMove* MoveBehaviour[NUM_PLAYER_TYPE];
	
	enum ePlayerType
	{
		ePlayerType_God,
		ePlayerType_FPS
	};

	ePlayerType m_eCurrentPlayerType;

	FRotator CurrentRotation{ FRotator::ZeroRotator };

public:

	ATwinStickShooterPawn();

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	void OnToggleGravity();
	void OnSwitchPlayerMode();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type) override;

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

public:

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
};


struct Base_PlayerMove
{
	virtual void operator()(ATwinStickShooterPawn* InPawn) = 0;// { QUICK_LOG_UPDATE(TEXT("BASE MOVE")); return InTransform; }
};

struct FPS_PlayerMove : Base_PlayerMove
{
	void operator()(ATwinStickShooterPawn* InPawn);//

};

struct God_PlayerMove : Base_PlayerMove
{
	void operator()(ATwinStickShooterPawn* InPawn);
};