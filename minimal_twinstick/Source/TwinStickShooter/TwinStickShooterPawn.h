// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TwinStickShooter.h"
#include "TwinStickShooterPawn.generated.h"

#define NUM_PLAYER_TYPE 2

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FPlayerDead );

class ATwinStickShooterProjectile;
class USphereComponent;

UENUM()
enum class EPlayerType : uint8
{
	ePlayerType_God,
	ePlayerType_FPS
};

enum EMenuWidgetState
{
	eMenuWidgetState_Closed = 0,
	eMenuWidgetState_Opening,
	eMenuWidgetState_Open,
	eMenuWidgetState_Closing
};

struct MenuWidgetInterface
{
	EMenuWidgetState m_eMenuWidgetState;// = eMenuWidgetState_Closed;
	float m_fMenuOpenAlpha = 0.0f;
	static const FVector m_vOpenScale;// = FVector2D(0.5f, 0.5f);
	static const int32 m_fOpenAnimSpeed = 10.0f;
};

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

	// Forward declaring friend struct instead of using getters and setters
	friend struct Base_PlayerMove;
	friend struct FPS_PlayerMove;
	friend struct God_PlayerMove;

	Base_PlayerMove* MoveBehaviour[NUM_PLAYER_TYPE];
	
	UPROPERTY(Replicated)
	EPlayerType m_eCurrentPlayerType;

	UPROPERTY(Replicated)
	FRotator SimulatedRotation;

	UPROPERTY(Replicated)
	FRotator CurrentRotation{ FRotator::ZeroRotator };
	MenuWidgetInterface MainMenuInterface;

	void OnOpenMenu();
	void UpdateMenuOpenAnimation(enum EMenuWidgetState AnimType);
	void UpdatePlayerBehaviour();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	ATwinStickShooterPawn();

	// ====== REPLICATION TEST ===================
	UPROPERTY(Replicated)
	int32 ReplicationTestCount = 0;

	UPROPERTY(Replicated)
	FVector MoveDirectionBuffer = FVector::ZeroVector;

	UFUNCTION(Server, unreliable, WithValidation) 
	void Server_IncreaseVariable();

	UFUNCTION(Server, reliable, WithValidation)
	void Server_ReplicateRotation(FRotator InRotation);

	UFUNCTION(Server, reliable, WithValidation)
	void Server_RequestMove(FVector InDirection);

	UFUNCTION()
	void OnRep_Count();
	// ===========================================

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* MenuWidget = nullptr;
	class UWidgetInteractionComponent* WidgetInteractionComponent;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	void OnToggleGravity();
	void OnSwitchPlayerMode();
	void AnimatePlayerTransform();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type) override;

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	FVector GetMoveDirection();

public:

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE void SetPawnBehaviour(const EPlayerType& In) { m_eCurrentPlayerType = In; }
};

struct Base_PlayerMove
{
	virtual void operator()(ATwinStickShooterPawn* InPawn) = 0;
};

struct FPS_PlayerMove : Base_PlayerMove
{
	void operator()(ATwinStickShooterPawn* InPawn);
};

struct God_PlayerMove : Base_PlayerMove
{
	void operator()(ATwinStickShooterPawn* InPawn);
};