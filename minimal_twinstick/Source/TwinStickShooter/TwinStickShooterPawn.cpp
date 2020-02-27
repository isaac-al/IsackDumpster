// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TwinStickShooterPawn.h"
#include "TwinStickShooterProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FlockEmitter.h"
#include "Components/CapsuleComponent.h"
#include "TwinstickShooter.h"
#include "Engine/Engine.h"

const FName ATwinStickShooterPawn::MoveForwardBinding("MoveForward");
const FName ATwinStickShooterPawn::MoveRightBinding("MoveRight");
const FName ATwinStickShooterPawn::FireForwardBinding("FireForward");
const FName ATwinStickShooterPawn::FireRightBinding("FireRight");

bool bGravityEnabled = true;

ATwinStickShooterPawn::ATwinStickShooterPawn()
{		
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/Meshes/SM_Ship.SM_Ship"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CamerComponent"));
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm
	CameraComponent->AttachTo(RootComponent);
	// Movement
	MoveSpeed = 120000.0f;

	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetEnableGravity(bGravityEnabled);
	CapsuleComponent->SetLinearDamping(10.0f);
	CapsuleComponent->SetAngularDamping(3.0f);
	CapsuleComponent->BodyInstance.bLockTranslation = false;
	//CapsuleComponent->BodyInstance.bLockZTranslation = false;
	CapsuleComponent->SetConstraintMode(EDOFMode::SixDOF);
	CapsuleComponent->BodyInstance.bLockZRotation = true;
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->SetMassOverrideInKg(NAME_None, 62, true);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetHiddenInGame(false);
	CapsuleComponent->SetVisibility(true);

	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void ATwinStickShooterPawn::BeginPlay()
{
	Super::BeginPlay();

	AFlockEmitter* const flock_default = GetWorld()->SpawnActor<AFlockEmitter>(FActorSpawnParameters());
	FBoidState default_state;
									//chase //sep //cohere //align //wander //scuttle //shape
	default_state.Weights = FWeights(0.f, .08f, .7f, 0.02f, 0.0f, 0.0f, 0.0f);
	default_state.RadiusFromTarget = 0.0f;
	default_state.bIsPlayer = true;
	flock_default->Init(EEnemyBehavaiour::eNavigate | EEnemyBehavaiour::eShoot, default_state, 128, FLinearColor::Blue, .5f, 200);
}

void ATwinStickShooterPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");
	PlayerInputComponent->BindAxis("FlyUp");

	PlayerInputComponent->BindAction("ToggleGravity", IE_Pressed, this, &ThisClass::OnToggleGravity);

	FInputModeGameOnly input;
	input.SetConsumeCaptureMouseDown(true);
	
	Cast<APlayerController>(GetController())->SetInputMode(input);
}

void ATwinStickShooterPawn::OnToggleGravity()
{
	CapsuleComponent->SetEnableGravity(bGravityEnabled = !bGravityEnabled);
}

FRotator CurrentRotation{FRotator::ZeroRotator};

void ATwinStickShooterPawn::Tick(float DeltaSeconds)
{	
	Super::Tick(DeltaSeconds);

	// ====== Camera Rotation ========================
	float y{ GetInputAxisValue("LookUp") };
	float x{ GetInputAxisValue("LookRight") };
	CurrentRotation.Pitch += y;
	CurrentRotation.Yaw += x;

	CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch, -45.0f, 45.0f);
	CameraComponent->SetRelativeRotation(CurrentRotation);
	// ===============================================
	
	// ====== Pawn Translation =======================
	float move_x{GetInputAxisValue(MoveForwardBinding)};
	float move_y{GetInputAxisValue(MoveRightBinding)};
	FRotator forward_rot = CurrentRotation;
	forward_rot.Roll  = 0;
	forward_rot.Pitch = 0;
	FVector rotation_vector = forward_rot.Vector();
	FVector dir{(rotation_vector * move_x) + (rotation_vector.RotateAngleAxis(90.0f, FVector::UpVector) * move_y)};
	dir.Normalize();
	Cast<UPrimitiveComponent>(RootComponent)->BodyInstance.AddForce(dir* MoveSpeed);
	// ===============================================

	if (!bGravityEnabled)
	{
		float move_z{ GetInputAxisValue("FlyUp") };
		FVector fly_dir = FVector::UpVector;
		Cast<UPrimitiveComponent>(RootComponent)->BodyInstance.AddForce(fly_dir * (move_z * MoveSpeed));
	}
}