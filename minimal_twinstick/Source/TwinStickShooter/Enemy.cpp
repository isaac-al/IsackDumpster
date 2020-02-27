// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "UObject/ConstructorHelpers.h"
#include "TwinStickShooterProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "EngineUtils.h"
#include "TwinStickShooterPawn.h"


// Sets default values
AEnemy::AEnemy() : CurrentHealth(DefaultHealth), EnemyType(EEnemyType::ET_Basic)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EnemySphereComp"));
	CollisionSphere->SetSphereRadius(400.0f);
	CollisionSphere->SetVisibility(true);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionProfileName(FName("OverlapAll"));
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	CollisionSphere->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tMesh(TEXT("/Game/Meshes/SM_Fly.SM_Fly"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tShooterMesh(TEXT("/Game/Meshes/SM_Shooter.SM_Shooter"));

	// Create the mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));

	RootComponent = MeshComponent;

	if (tMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(tMesh.Object);
		BasicMesh = tMesh.Object;
	}

	if (tShooterMesh.Succeeded())
	{
		ShooterMesh = tShooterMesh.Object;
	}

	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(false);
	MeshComponent->BodyInstance.SetCollisionProfileName("Enemy");
	MeshComponent->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	MeshComponent->AddLocalRotation(FQuat::MakeFromEuler(FVector(0,-90,0)));
	MeshComponent->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlap);
	CollisionSphere->SetupAttachment(RootComponent);

	OtherEnemyArray = TArray<AActor*>();
	DesiredForce = FVector::ZeroVector;

	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.4f;
	bCanFire = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> tProj(TEXT("/Game/Blueprints/BP_EnemyProjectile.BP_EnemyProjectile"));
	
	if (tProj.Succeeded())
		ProjectileClass = (UClass*)tProj.Object->GeneratedClass;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();	

	switch (EnemyType)
	{
	case EEnemyType::ET_Basic:
		//CurrentHealth = 
		break;
	case EEnemyType::ET_Shooter:

		break;
	}
}

//bool bMove = true;
// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool bMove = true;

	auto tPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	FVector tSeekForce = GetActorLocation();

	if (!tPlayer) return;

	switch (EnemyType)
	{
	case EEnemyType::ET_Basic:
		tSeekForce = Seek(tPlayer->GetActorLocation());

		break;
	case EEnemyType::ET_Shooter:

		// TODO : Move logic from switch into function!!

		/* Set seek target to 300 - player pos */
		FVector toff = tPlayer->GetVelocity();

		toff.Normalize();
		toff *= 200.0f;

		SeekWeight = 5.0f;
		WanderWeight = 0;
		ScutterWeight = 0;
		SeparateWeight = 15.0f;
		
		FVector tDisp = tPlayer->GetActorLocation() - GetActorLocation();
		tDisp.Normalize();
		FVector FireDirection = tDisp;
		// Try and fire a shot when within x distance of player

		float tDist = FVector::Distance(GetActorLocation(), tPlayer->GetActorLocation());
		float FiringDistance = 500;

		FVector tSeekTarget = tDisp * -FiringDistance;
		FVector tSeek = tPlayer->GetActorLocation() + tSeekTarget;
		tSeekForce = Seek(tSeek);

		//DrawDebugSphere(GetWorld(), tSeek, 30, 16, FColor::Red);

		if (FMath::Abs(tDist) <= FiringDistance)
		{
			//SeekWeight = -1;
			//MeshComponent->AddImpulse(-GetVelocity());
			//MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
			//bMove = false;
		}

		if (tDist < FiringDistance * 2)
		{
			FireShot(FireDirection, DeltaTime);
		}
		
		break;
	}

	// Find all enemy actors within bounds
	GetOverlappingActors(OtherEnemyArray, TSubclassOf<AEnemy>());

	FVector tSctrForce = Scutter(DeltaTime);
	FVector tWndrForce = Wander();
	
	FVector tSepForce = Separate();
	FVector tAlignForce = Align();
	FVector tCohForce = Cohere();

	tSctrForce *= ScutterWeight;
	tWndrForce *= WanderWeight;
	tSeekForce *= SeekWeight;
	tSepForce *= SeparateWeight;
	tAlignForce *= AlignWeight;
	tCohForce *= CohereWeight;

	FVector tTotalForce = tSctrForce + tWndrForce + tSeekForce + tSepForce + tAlignForce + tCohForce;

	tTotalForce.GetClampedToSize(1, MaxForce);

	if (bMove)
		MeshComponent->AddForce(tTotalForce);

	MeshComponent->SetPhysicsLinearVelocity(FVector(GetVelocity()).GetClampedToMaxSize(MaxSpeed));

	FVector vel = GetVelocity();
	FVector tRotDir = GetActorLocation() + vel;
	FRotator tRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), tRotDir);
	MeshComponent->SetRelativeRotation(tRot);
}

void AEnemy::FireShot(FVector FireDirection, float DeltaTime)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			// If we are pressing fire stick in a direction
			if (FireDirection.SizeSquared() > 0.0f)
			{
				const FRotator FireRotation = FireDirection.Rotation();
				FRotator tDiffYaw = FRotator(GetActorRotation().Pitch, FireRotation.Yaw, GetActorRotation().Roll);
				FRotator tDeltaYaw = FMath::RInterpTo(GetActorRotation(), tDiffYaw, DeltaTime, 0.f);

				// Spawn projectile at an offset from this pawn
				const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					// spawn the projectile
					//ProjectileClass
					World->SpawnActor<ATwinStickShooterProjectile>(ProjectileClass, SpawnLocation, FireRotation);
				}

				bCanFire = false;
				World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AEnemy::ShotTimerExpired, FireRate);

				bCanFire = false;
			}
		}
	}
}

void AEnemy::ShotTimerExpired()
{
	bCanFire = true;
}

FVector AEnemy::Steer(FVector pDir)
{
	pDir.Normalize();
	pDir = pDir * MaxSpeed;
	FVector tDir = pDir - GetVelocity();
	tDir.GetClampedToSize(1, MaxForce);
	
	return tDir;
}

void AEnemy::Init(EEnemyType pType, float pSctr, float pWndr, float pSeek, float pSep, float pAlign, float pCoh)
{
	EnemyType = pType;
	switch (pType)
	{
	case EEnemyType::ET_Shooter:
		/* Set mesh to shooter mesh */
		/* Maybe alter some weights to change behaviour */

		WanderWeight = 0;
		ScutterWeight = 0;

		MeshComponent->SetStaticMesh(ShooterMesh);
		break;
	default:
		break;
	}

	ScutterWeight = pSctr;
	WanderWeight = pWndr;
	SeekWeight = pSeek;
	SeparateWeight = pSep;
	AlignWeight = pAlign;
	CohereWeight = pCoh;
}

void AEnemy::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->IsChildOf(AEnemy::StaticClass()))
	{
		OtherEnemyArray.Add(OtherActor);		
	}
}

void AEnemy::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->IsChildOf(AEnemy::StaticClass()))
	{
		OtherEnemyArray.Remove(OtherActor);
	}
}

FVector AEnemy::Wander()
{
	float tWanderR = 500;
	float tWanderD = 400.0f;
	float tRange = 0.5f;

	WanderTheta += FMath::RandRange(-tRange, tRange);

	// Make wander circle 
	FVector tCircLoc = GetVelocity();
	tCircLoc.Normalize();
	tCircLoc *= tWanderD;
	tCircLoc += GetActorLocation();

	float h = GetVelocity().HeadingAngle();

	FVector tCircOff = FVector(tWanderR*cos(WanderTheta+h), tWanderR*sin(WanderTheta + h), 0);
	FVector tDesired = tCircLoc + tCircOff;

	return Seek(tDesired);
}

FVector AEnemy::Scutter(float DeltaTime)
{
	float tWanderR = 500;
	float tWanderD = 400.0f;
	float tRange = 0.5f;

	float ScutterSpeed = 10.0f;

	FVector tCircLoc = GetVelocity();
	tCircLoc.Normalize();
	tCircLoc *= tWanderD;
	tCircLoc += GetActorLocation();

	float h = GetVelocity().HeadingAngle();

	FVector tCircOff = FVector(tWanderR*cos(h+sin(FPlatformTime::Seconds() * 10.0f)), tWanderR*sin(h+sin(FPlatformTime::Seconds() * 10.0f)), 0);
	FVector tDesired = tCircLoc + tCircOff;

	return Seek(tDesired);
}

FVector AEnemy::Align()
{
	FVector tSteer = FVector::ZeroVector;
	int tCount = 0;

	for (auto tActor : OtherEnemyArray)
	{
		if (!tActor->IsA(ATwinStickShooterPawn::StaticClass()))
		{
			tCount++;
			tSteer += tActor->GetVelocity();
		}
	}

	if (tCount > 0)
	{
		tSteer /= (float)tCount;
	}

	return Steer(tSteer);
}

FVector AEnemy::Cohere()
{
	FVector tSteer = FVector::ZeroVector;
	int tCount = 0;

	for (auto tActor : OtherEnemyArray)
	{
		if (!tActor->IsA(ATwinStickShooterPawn::StaticClass()))
		{
			tCount++;

			tSteer += tActor->GetActorLocation();
		}
	}

	if (tCount > 0)
	{
		tSteer /= (float)tCount;
	}

	return Steer(tSteer);
}

FVector AEnemy::Seek(FVector pTarget)
{
	FVector tTargetPos = pTarget;
	FVector tDesired = tTargetPos - GetActorLocation();
	return Steer(tDesired);
}

FVector AEnemy::Separate()
{
	float tCount = 0;
	FVector tFTotal = FVector::ZeroVector;
	// For each enemy in enemy list, add force via weighted velocity
	for (auto tActor : OtherEnemyArray)
	{
		tCount++;

		if (!tActor->IsA(ATwinStickShooterPawn::StaticClass()))
		{
			float tDist = FVector::Distance(GetActorLocation(), tActor->GetActorLocation());
			float tSepMin = 50.0f;
			if (tDist < tSepMin)
			{
				FVector tDiff = GetActorLocation() - tActor->GetActorLocation();
				tDiff.Normalize();
				tDiff = tDiff / tDist;
				tFTotal += tDiff;

				DrawDebugLine(
					GetWorld(),
					GetActorLocation(),
					tActor->GetActorLocation(),
					FColor(0, 143, 0),
					false, -1, 0,
					2
				);
			}
		}
	}

	if (tCount > 0)
	{
		tFTotal /= (float)tCount;
	}

	return Steer(tFTotal);
}

void AEnemy::AddTotalDesiredForce(FVector pForce)
{
	if (pForce.IsNormalized())
	{
		DesiredForce += pForce;
	}
	else
	{
		pForce.Normalize();
		DesiredForce += pForce;
	}
}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void AEnemy::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Damage this actor 
	if (OtherActor->IsA(ATwinStickShooterPawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PAWN DAMAGE"));
		TSubclassOf<UDamageType> tDamageType;
		UGameplayStatics::ApplyDamage(OtherActor, 10.0f, this->GetInstigatorController(), this, tDamageType);
		Destroy();

	}

}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DetractHealth(DamageAmount);

	return 0.0f;
}

void AEnemy::DetractHealth(float pAmount)
{
	CurrentHealth -= pAmount;

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}

}
