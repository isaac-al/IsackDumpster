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
#include "Engine/StaticMeshActor.h"

const FName ATwinStickShooterPawn::MoveForwardBinding("MoveForward");
const FName ATwinStickShooterPawn::MoveRightBinding("MoveRight");
const FName ATwinStickShooterPawn::FireForwardBinding("FireForward");
const FName ATwinStickShooterPawn::FireRightBinding("FireRight");

bool bGravityEnabled = true;
float m_fDeltaTime = 0.0f;

ATwinStickShooterPawn::ATwinStickShooterPawn() : m_eCurrentPlayerType(ePlayerType_FPS)
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
	default_state.Weights = FWeights(1.0f, .1f, .1f, .0f, .0f, .0f, .0f);
	default_state.RadiusFromTarget = 0.0f;
	default_state.bIsPlayer = true;

	MoveBehaviour[ePlayerType_FPS] = new FPS_PlayerMove();
	MoveBehaviour[ePlayerType_God] = new God_PlayerMove();

	TArray<AActor*> FoundMeshes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), FoundMeshes);
	AActor* TargetMesh = nullptr;

	for (int32 i = 0; i < FoundMeshes.Num(); i++)
	{
		if (FoundMeshes[i]->GetName().Contains(TEXT("BeeHive")))
		{
			TargetMesh = FoundMeshes[i];
		}
	}

	flock_default->SetActorLocation(TargetMesh->GetActorLocation());
	flock_default->Init(EEnemyBehavaiour::eNavigate | EEnemyBehavaiour::eShoot, default_state, 128, FLinearColor::Blue, .3f, 75, TargetMesh);
	m_fDeltaTime = 0.0f;
}

void ATwinStickShooterPawn::EndPlay(EEndPlayReason::Type)
{
	for (int32 player_type = 0; player_type < NUM_PLAYER_TYPE; player_type++)
	{
		delete MoveBehaviour[player_type];
	}
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
	PlayerInputComponent->BindAction("SwitchMode", IE_Pressed, this, &ThisClass::OnSwitchPlayerMode);

	FInputModeGameOnly input;
	input.SetConsumeCaptureMouseDown(true);
	
	Cast<APlayerController>(GetController())->SetInputMode(input);
}

void ATwinStickShooterPawn::OnToggleGravity()
{
	CapsuleComponent->SetEnableGravity(bGravityEnabled = !bGravityEnabled);
}

FTransform PrevTransform;
FTransform CurrentTransform;
bool m_bPlayerModeAnimate = false;
float m_fPlayerModeAlpha = 0.0f;

void ATwinStickShooterPawn::OnSwitchPlayerMode()
{
	m_eCurrentPlayerType = ePlayerType((1 + m_eCurrentPlayerType + NUM_PLAYER_TYPE) % NUM_PLAYER_TYPE);

	PrevTransform.SetLocation(GetActorLocation());
	PrevTransform.SetRotation(CameraComponent->GetComponentRotation().Quaternion());

	m_bPlayerModeAnimate = false;
	m_fPlayerModeAlpha = 0.0f;

	TArray<FHitResult> hit;

	float half_size = 1000000;
	FVector cast_start = PrevTransform.GetLocation() + (FVector::UpVector * half_size);
	FVector cast_end = PrevTransform.GetLocation() + (FVector::UpVector * -half_size);
	GetWorld()->LineTraceMultiByObjectType(hit, cast_start, cast_end, ECC_WorldStatic);

	// Get hit location from ground
	FVector ground_location = hit.Last().ImpactPoint;

	switch (m_eCurrentPlayerType)
	{
	case ATwinStickShooterPawn::ePlayerType_God:
	{
		MoveSpeed = 520000.0f;
		CurrentTransform.SetLocation(FVector(GetActorLocation().X, GetActorLocation().Y, ground_location.Z + 1720.0f));
		CurrentTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.0f, -45.0f, 0.0f)));
		CapsuleComponent->SetEnableGravity(false);
	} break;
	case ATwinStickShooterPawn::ePlayerType_FPS:
	{
		MoveSpeed = 120000.0f;
		CurrentTransform.SetLocation(ground_location + (FVector::UpVector * 100));
		CurrentTransform.SetRotation(FQuat::MakeFromEuler(FVector::ZeroVector));
	} break;
	default: break;
	}

	m_bPlayerModeAnimate = true;
}

void ATwinStickShooterPawn::Tick(float DeltaSeconds)
{	
	m_fDeltaTime = DeltaSeconds;
	Super::Tick(DeltaSeconds);
	
	if (m_bPlayerModeAnimate)
	{
		// Animate Transform!
		m_fPlayerModeAlpha += m_fDeltaTime;

		float result_alpha = UKismetMathLibrary::Ease(0.0f, 1.0f, m_fPlayerModeAlpha, EEasingFunc::EaseOut, 5);

		FQuat interp_rot = FQuat::Slerp(PrevTransform.GetRotation(), CurrentTransform.GetRotation(), result_alpha);
		FVector interp_loc = FMath::Lerp(PrevTransform.GetLocation(), CurrentTransform.GetLocation(), result_alpha);
		CameraComponent->SetRelativeRotation(interp_rot);
		SetActorLocation(interp_loc);

		if (m_fPlayerModeAlpha >= 1.0f)
		{
			m_bPlayerModeAnimate = false;
			CurrentRotation = CameraComponent->GetRelativeRotation();
		}
	}
	else
	{
		(*MoveBehaviour[m_eCurrentPlayerType])(this);
	}
}

void FPS_PlayerMove::operator()(ATwinStickShooterPawn* InPawn) 
{
	ATwinStickShooterPawn& p = (*InPawn);

	//// ====== Camera Rotation ========================
	float y{ InPawn->GetInputAxisValue("LookUp") };
	float x{ InPawn->GetInputAxisValue("LookRight") };
	p.CurrentRotation.Pitch += y;
	p.CurrentRotation.Yaw += x;
	p.CurrentRotation.Pitch = FMath::Clamp(p.CurrentRotation.Pitch, -45.0f, 45.0f);
	p.CameraComponent->SetRelativeRotation(p.CurrentRotation);
	//// ===============================================

	//// ====== Pawn Translation =======================
	float move_x{ p.GetInputAxisValue(p.MoveForwardBinding) };
	float move_y{ p.GetInputAxisValue(p.MoveRightBinding) };
	FRotator forward_rot = p.CurrentRotation;
	forward_rot.Roll = 0;
	forward_rot.Pitch = 0;
	FVector rotation_vector = forward_rot.Vector();
	FVector dir{ (rotation_vector * move_x) + (rotation_vector.RotateAngleAxis(90.0f, FVector::UpVector) * move_y) };
	dir.Normalize();
	Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(dir * p.MoveSpeed);
	//// ===============================================

	extern bool bGravityEnabled;

	if (!bGravityEnabled)
	{
		float move_z{ p.GetInputAxisValue("FlyUp") };
		FVector fly_dir = FVector::UpVector;
		Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(fly_dir * (move_z * p.MoveSpeed));
	}
}

void God_PlayerMove::operator()(ATwinStickShooterPawn* InPawn)
{
	ATwinStickShooterPawn& p = (*InPawn);

	//// ====== Pawn Translation =======================
	float move_x{ p.GetInputAxisValue(p.MoveForwardBinding) };
	float move_y{ p.GetInputAxisValue(p.MoveRightBinding) };
	FRotator forward_rot = p.CurrentRotation;
	forward_rot.Roll = 0;
	forward_rot.Pitch = 0;
	FVector rotation_vector = forward_rot.Vector();
	FVector dir{ (rotation_vector * move_x) + (rotation_vector.RotateAngleAxis(90.0f, FVector::UpVector) * move_y) };
	dir.Normalize();
	Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(dir * p.MoveSpeed);
	//// ===============================================
}
