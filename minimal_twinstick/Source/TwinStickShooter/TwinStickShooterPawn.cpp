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
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Components/WidgetInteractionComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Engine/Public/Net/UnrealNetwork.h"

const FName ATwinStickShooterPawn::MoveForwardBinding("MoveForward");
const FName ATwinStickShooterPawn::MoveRightBinding("MoveRight");
const FName ATwinStickShooterPawn::FireForwardBinding("FireForward");
const FName ATwinStickShooterPawn::FireRightBinding("FireRight");

bool bGravityEnabled = true;
float m_fDeltaTime = 0.0f;

const FVector MenuWidgetInterface::m_vOpenScale = FVector(0.12f, 0.12f, 0.15f);

void ATwinStickShooterPawn::UpdatePlayerBehaviour()
{
	(*MoveBehaviour[(uint8)m_eCurrentPlayerType])(this);
}

void ATwinStickShooterPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATwinStickShooterPawn, m_eCurrentPlayerType);
	DOREPLIFETIME(ATwinStickShooterPawn, ReplicationTestCount);
	DOREPLIFETIME(ATwinStickShooterPawn, SimulatedRotation);
	DOREPLIFETIME(ATwinStickShooterPawn, MoveDirectionBuffer);
}

bool ATwinStickShooterPawn::Server_IncreaseVariable_Validate()
{
	return true;
}

void ATwinStickShooterPawn::Server_IncreaseVariable_Implementation()
{
	ReplicationTestCount++;
}

bool ATwinStickShooterPawn::Server_ReplicateRotation_Validate(FRotator InRotation)
{
	return true;
}

void ATwinStickShooterPawn::Server_ReplicateRotation_Implementation(FRotator InRotation)
{
	SimulatedRotation = InRotation;
}

bool ATwinStickShooterPawn::Server_RequestMove_Validate(FVector InWorldLocation)
{
	return true;
}

void ATwinStickShooterPawn::Server_RequestMove_Implementation(FVector InDirection)
{
	MoveDirectionBuffer = InDirection;
	// Normalize to prevent cheating
	MoveDirectionBuffer.Normalize();
}

FVector ATwinStickShooterPawn::GetMoveDirection()
{
	float move_x{ GetInputAxisValue(MoveForwardBinding) };
	float move_y{ GetInputAxisValue(MoveRightBinding) };
	FRotator forward_rot = CurrentRotation;
	forward_rot.Roll = 0;
	forward_rot.Pitch = 0;
	FVector rotation_vector = forward_rot.Vector();
	FVector dir{ (rotation_vector * move_x) + (rotation_vector.RotateAngleAxis(90.0f, FVector::UpVector) * move_y) };
	dir.Normalize();
	return dir;
}

void ATwinStickShooterPawn::UpdateMenuOpenAnimation(EMenuWidgetState AnimType)
{
	if (Role == ROLE_AutonomousProxy || IsNetMode(NM_Standalone))
	{
		int32 dir = AnimType == eMenuWidgetState_Opening ? 1 : -1;
		FVector current_scale = MenuWidget->GetRelativeScale3D();
		FVector EndScale = MenuWidgetInterface::m_vOpenScale;
		FVector StartScale = FVector(EndScale.X, EndScale.Y, 0.0f);

		float d = m_fDeltaTime * MenuWidgetInterface::m_fOpenAnimSpeed;
		MainMenuInterface.m_fMenuOpenAlpha = FMath::Clamp(MainMenuInterface.m_fMenuOpenAlpha + (d * dir), 0.0f, 1.0f);
		float result_alpha = UKismetMathLibrary::Ease(0.0f, 1.0f, MainMenuInterface.m_fMenuOpenAlpha, EEasingFunc::EaseInOut, 3);
		FVector interp_loc = FMath::Lerp(StartScale, EndScale, result_alpha);
		MenuWidget->SetRelativeScale3D(interp_loc);

		if (MainMenuInterface.m_fMenuOpenAlpha <= 0 && dir < 0)
		{
			MainMenuInterface.m_eMenuWidgetState = eMenuWidgetState_Closed;
		}
		else if (MainMenuInterface.m_fMenuOpenAlpha >= 1 && dir > 0)
		{
			MainMenuInterface.m_eMenuWidgetState = eMenuWidgetState_Open;
		}
	}
}

void ATwinStickShooterPawn::OnOpenMenu()
{
	switch (MainMenuInterface.m_eMenuWidgetState)
	{
	case eMenuWidgetState_Closed:
	case eMenuWidgetState_Closing:
		MainMenuInterface.m_eMenuWidgetState = eMenuWidgetState_Opening;
		break;
	case eMenuWidgetState_Open:
	case eMenuWidgetState_Opening:
		MainMenuInterface.m_eMenuWidgetState = eMenuWidgetState_Closing;
	break;
	}
}

ATwinStickShooterPawn::ATwinStickShooterPawn() : m_eCurrentPlayerType(EPlayerType::ePlayerType_FPS)
{		
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/Meshes/SM_Ship.SM_Ship"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	MenuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MenuWidgetComponent"));
	MenuWidget->AttachTo(RootComponent);
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComponent"));

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

	bReplicateMovement = true;
	bReplicates = true;
}

void ATwinStickShooterPawn::OnRep_Count()
{
	ensure(false);
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

	MoveBehaviour[(uint8)EPlayerType::ePlayerType_FPS] = new FPS_PlayerMove();
	MoveBehaviour[(uint8)EPlayerType::ePlayerType_God] = new God_PlayerMove();

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
	MenuWidget->SetRelativeScale3D(FVector::ZeroVector);
	UClass* widget_class = StaticLoadClass(UUserWidget::StaticClass(), NULL, TEXT("/Game/Widgets/BP_MainMenu.BP_MainMenu_C"));
	UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), widget_class, TEXT("MainMenu"));
	MenuWidget->SetWidget(widget);
	MenuWidget->SetWindowFocusable(true);

	WidgetInteractionComponent->InteractionDistance = 10000.0f;
	WidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::CenterScreen;
	WidgetInteractionComponent->bEnableHitTesting = true;
	WidgetInteractionComponent->bShowDebug = true;
	WidgetInteractionComponent->SetRelativeLocation(FVector::ForwardVector * 100);
	MainMenuInterface.m_eMenuWidgetState = eMenuWidgetState_Closed;
	MainMenuInterface.m_fMenuOpenAlpha = 0.0f;
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
	PlayerInputComponent->BindAction("OpenMenu", IE_Released, this, &ThisClass::OnOpenMenu);

	FInputModeGameOnly input;
	input.SetConsumeCaptureMouseDown(true);
	
	Cast<APlayerController>(GetController())->SetInputMode(input);
}

void ATwinStickShooterPawn::OnToggleGravity()
{
	CapsuleComponent->SetEnableGravity(bGravityEnabled = !bGravityEnabled);
	WidgetInteractionComponent->PressAndReleaseKey(EKeys::LeftMouseButton);
}

FTransform PrevTransform;
FTransform CurrentTransform;
bool m_bPlayerModeAnimate = false;
float m_fPlayerModeAlpha = 0.0f;

void ATwinStickShooterPawn::OnSwitchPlayerMode()
{
	m_eCurrentPlayerType = EPlayerType((1 + (uint8)m_eCurrentPlayerType + NUM_PLAYER_TYPE) % NUM_PLAYER_TYPE);

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
	case EPlayerType::ePlayerType_God:
	{
		MoveSpeed = 520000.0f;
		CurrentTransform.SetLocation(FVector(GetActorLocation().X, GetActorLocation().Y, ground_location.Z + 1720.0f));
		CurrentTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.0f, -45.0f, 0.0f)));
		CapsuleComponent->SetEnableGravity(false);
	} break;
	case EPlayerType::ePlayerType_FPS:
	{
		MoveSpeed = 120000.0f;
		CurrentTransform.SetLocation(ground_location + (FVector::UpVector * 100));
		CurrentTransform.SetRotation(FQuat::MakeFromEuler(FVector::ZeroVector));
	} break;
	default: break;
	}

	m_bPlayerModeAnimate = true;
}

void ATwinStickShooterPawn::AnimatePlayerTransform()
{
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

void ATwinStickShooterPawn::Tick(float DeltaSeconds)
{	
	m_fDeltaTime = DeltaSeconds;
	Super::Tick(DeltaSeconds);
	
	if (m_bPlayerModeAnimate)
	{
		// Animate Transform!
		AnimatePlayerTransform();
	}
	else
	{
		FVector cam_forward = CameraComponent->GetForwardVector();
		cam_forward.Z = 0.0f;

		FRotator menu_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + cam_forward * 200.0f);
		menu_rotation += FRotator::MakeFromEuler(FVector(0.0f,0.0f,180.0f));
		FVector menu_location = cam_forward * 200.0f;
		MenuWidget->SetRelativeRotation(menu_rotation);
		MenuWidget->SetRelativeLocation(menu_location);
		DrawDebugBox(GetWorld(), GetActorLocation() + CameraComponent->GetForwardVector() * 200.0f, FVector(50.0f, 100.0f, 50.0f), MenuWidget->GetRelativeRotation().Quaternion(), FColor::Emerald, false, -1, 0, 10.0f);

		switch (MainMenuInterface.m_eMenuWidgetState)
		{
		case eMenuWidgetState_Closed:
		{
			UpdatePlayerBehaviour();
		} break;
		case eMenuWidgetState_Open:
		{
			// Poll menu inputs etc.
		} break;
		case eMenuWidgetState_Opening:
		{
			UpdateMenuOpenAnimation(eMenuWidgetState_Opening);
		} break;
		case eMenuWidgetState_Closing:
		{
			UpdateMenuOpenAnimation(eMenuWidgetState_Closing);
		} break;
		default: break;
		}
	}
}

void FPS_PlayerMove::operator()(ATwinStickShooterPawn* InPawn) 
{
	ATwinStickShooterPawn& p = (*InPawn);
	
	auto local_role = p.GetLocalRole();
	auto remote_role = p.GetRemoteRole();
	
	if (local_role == ROLE_AutonomousProxy || p.IsNetMode(NM_Standalone))
	{
		//// ====== Camera Rotation ========================
		float y{ p.GetInputAxisValue("LookUp") };
		float x{ p.GetInputAxisValue("LookRight") };
		p.CurrentRotation.Pitch += y;
		p.CurrentRotation.Yaw += x;
		p.CurrentRotation.Pitch = FMath::Clamp(p.CurrentRotation.Pitch, -45.0f, 45.0f);
		p.CameraComponent->SetRelativeRotation(p.CurrentRotation);
		p.Server_ReplicateRotation(p.CameraComponent->GetRelativeRotation());
		//// ===============================================
	}
	else if (local_role == ROLE_SimulatedProxy)
	{
		p.CameraComponent->SetRelativeRotation(p.SimulatedRotation);
	}

	//// ====== Pawn Translation =======================
	if (p.GetNetMode() == NM_Standalone)
	{
		FVector dir = p.GetMoveDirection();
		Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(dir * p.MoveSpeed);

		if (!bGravityEnabled)
		{
			float move_z{ p.GetInputAxisValue("FlyUp") };
			FVector fly_dir = FVector::UpVector;
			Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(fly_dir * (move_z * p.MoveSpeed));
		}
	}
	else if (local_role == ROLE_Authority)
	{
		Cast<UPrimitiveComponent>(p.RootComponent)->BodyInstance.AddForce(p.MoveDirectionBuffer * p.MoveSpeed);
	}
	else if (local_role == ROLE_AutonomousProxy)
	{
		FVector dir = p.GetMoveDirection();
		p.Server_RequestMove(dir);
	}
	//// ===============================================
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

// TODO: 
// - Integrate physics on server x
// - Replicate other client pawn x
// - Smooth player movement

// === Player Movement Interpolation ===============
//	Player location (0,0,0)
//	Tick time (0.03)
//	Send requested move direction to server (0,1,0)
//	Packet transfer rate (0.05)
//	
// =================================================