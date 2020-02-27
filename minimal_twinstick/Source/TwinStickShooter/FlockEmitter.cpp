// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockEmitter.h"
#include "Components/StaticMeshComponent.h"
#include "TwinStickShooter.h"
#include "Kismet/GameplayStatics.h"
#include "TwinStickShooterPawn.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <string>
#include <cmath>
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#define STRIDE (uint8)8
#define BIT_SWITCH(x) \
	for (uint32 b = 1; x >= b; b *= 2) \
		if (x & b) switch (b)


FVector SphericalToCartesian(float theta, float phi, float radius)
{
	FVector tVec;

	tVec.X = std::cos(theta) * std::cos(phi) * radius;
	tVec.Y = std::sin(theta) * std::cos(phi) * radius;
	tVec.Z = std::sin(phi) * radius;

	return tVec;
}

// Sets default values
AFlockEmitter::AFlockEmitter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoidInstanceMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FlockMeshInstanceComponent"));

}

void AFlockEmitter::Init(EEnemyBehavaiour Behaviour, FBoidState InBoidState, int32 NumToSpawn, FLinearColor InColour, float InScale, int32 SpawnRangeRadius)
{
	int32 radius  = SpawnRangeRadius;
	BoidBehaviour = Behaviour;
	BoidMass      = InScale;
	FVector actorLoc = GetActorLocation();

#if 0
	for (int32 i = 0; i < NumToSpawn; i++)
	{
		UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this);
		mesh->RegisterComponent();
		mesh->ComponentTags.Empty();
		FName boid = FName(TEXT("boid"));
		mesh->ComponentTags.Add(std::move(boid));
		FString player_str(TEXT(""));
		std::string str = std::to_string(InBoidState.bIsPlayer);
		char* isplayer = &(str)[0];
		mesh->ComponentTags.Add(std::move(isplayer));
		FVector actorLoc = GetActorLocation();

		float randX = actorLoc.X + FMath::RandRange(-radius, radius);
		float randY = actorLoc.Y + FMath::RandRange(-radius, radius);
		float randZ = actorLoc.Z + FMath::RandRange(-radius, radius);

		UStaticMesh* sm = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Meshes/SM_BoidDefault.SM_BoidDefault'"));
		mesh->SetStaticMesh(sm);
		mesh->SetWorldLocation(FVector(randX, randY, randZ));
		BoidMesh.Add(mesh);
		BoidState.Init(InBoidState, NumToSpawn);
		FlockSize = NumToSpawn;
		UMaterialInstanceDynamic* mi = UMaterialInstanceDynamic::Create(mesh->GetMaterial(0), NULL);
		mesh->SetMaterial(0, mi);
		mi->SetVectorParameterValue(TEXT("BaseColour"), InColour);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mesh->SetRelativeScale3D(FVector::OneVector * InScale);
	}
#else
	
	UStaticMesh* sm = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Meshes/SM_BoidDefault.SM_BoidDefault'"));
	BoidInstanceMesh->SetStaticMesh(sm);

	UMaterialInstanceDynamic* mi = UMaterialInstanceDynamic::Create(BoidInstanceMesh->GetMaterial(0), NULL);
	//mi->SetVectorParameterValue(TEXT("BaseColour"), InColour);
	BoidInstanceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoidInstanceMesh->SetRelativeScale3D(FVector::OneVector * InScale);

	for (int32 i = 0; i < NumToSpawn; i++)
	{
		FTransform boid_transform{};
		float randX = actorLoc.X + FMath::RandRange(-radius, radius);
		float randY = actorLoc.Y + FMath::RandRange(-radius, radius);
		float randZ = actorLoc.Z + FMath::RandRange(-radius, radius);
		FVector start_loc{randX, randY, randZ};

		boid_transform.SetLocation(start_loc);
		BoidInstanceMesh->AddInstance(boid_transform);
		BoidState.Init(InBoidState, NumToSpawn);
	}

	BoidInstanceMesh->SetMaterial(0, mi);
	FlockSize = NumToSpawn;

#endif
}

// Optional radius to avoid collision with player
void AFlockEmitter::Chase(float DeltaTime, float Radius)
{
	UE_LOG(LogTemp, Warning, TEXT("Chase!"));

	#if 0
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		if (!BoidState[i].Target)
		{
			return;	
		}
		
		FVector target_loc = GetBoidWorldLocation(BoidState[i].Target);

		FVector loc = BoidState[i].Target->GetOwner()->GetActorLocation();

		const float offset_radius = -BoidState[i].RadiusFromTarget;
		FVector worldLocation = GetBoidWorldLocation(BoidMesh[i]);
		FVector direction = target_loc - worldLocation;

		FVector offset = target_loc + (direction.GetSafeNormal() * offset_radius);
		BoidState[i].TotalForce += Steer(i, (offset - worldLocation)) * BoidState[i].Weights.Chase;
		//DrawDebugLine(GetWorld(), worldLocation, (offset), FColor::Magenta, false, -1.0f, (uint8)'\000', 2.0f);
		DrawDebugSphere(GetWorld(), (offset), 10.0f, 6, FColor::Magenta);
	}
	#else

	for (int32 i = 0; i < BoidState.Num(); i++)
	{
		
	}
	#endif
}

void AFlockEmitter::Separate(float DeltaTime, float Radius)
{
#if 0
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		// add force in inverse direction of distance between a and b

		FVector f = FVector::ZeroVector;

		for (int32 j = 0; j < BoidMesh.Num(); j++)
		{
			if (i == j) continue;

			float r = FVector::Distance(BoidMesh[i]->RelativeLocation, BoidMesh[j]->RelativeLocation);
			const float min_dist = 200.0f;
			if (r < min_dist)
			{
				FVector disp = BoidMesh[i]->RelativeLocation -
					BoidMesh[j]->RelativeLocation;
				float dist = disp.Size();
				FVector dir = disp;
				f += dir / dist;
			}
		}

		BoidState[i].TotalForce += Steer(i, /*GetBoidWorldLocation(BoidMesh[i]) + */f) * BoidState[i].Weights.Seperate;
	}
#else

	auto get_other_dist{[this](const int32& other, const FTransform& boid_transform) -> FVector
	{
		FTransform otherboid_transform;
		BoidInstanceMesh->GetInstanceTransform(other, otherboid_transform, /*world space?*/false);

		float r = FVector::Distance(boid_transform.GetLocation(), otherboid_transform.GetLocation());
		const float min_dist = 200.0f;
		if (r < min_dist)
		{
			FVector disp = boid_transform.GetLocation() -
				otherboid_transform.GetLocation();
			float dist = disp.Size();
			FVector dir = disp;
			return dir / dist;
		}
	}};

	auto boid_separate {[&get_other_dist, this](const int32& boid)
	{
		FTransform boid_transform;
		BoidInstanceMesh->GetInstanceTransform(boid, boid_transform, /*world space?*/false);
		FVector f = FVector::ZeroVector;

		for (int32 j = 0; j < BoidState.Num(); j+= STRIDE)
		{
			f += get_other_dist(j,   boid_transform);
			f += get_other_dist(j+1, boid_transform);
			f += get_other_dist(j+2, boid_transform);
			f += get_other_dist(j+3, boid_transform);
			f += get_other_dist(j+4, boid_transform);
			f += get_other_dist(j+5, boid_transform);
			f += get_other_dist(j+6, boid_transform);
			f += get_other_dist(j+7, boid_transform);
		}

		BoidState[boid].TotalForce += Steer(f) * BoidState[boid].Weights.Seperate * m_PhysTime;
	}};

	#if 0
for (int32 i = 0; i < BoidState.Num(); i+= STRIDE)
	{
		FTransform boid_transform;
		BoidInstanceMesh->GetInstanceTransform(i, boid_transform, /*world space?*/false);

		FVector f = FVector::ZeroVector;

		for (int32 j = 0; j < BoidState.Num(); j++)
		{
			if (i == j) continue;

			FTransform otherboid_transform;
			BoidInstanceMesh->GetInstanceTransform(j, otherboid_transform, /*world space?*/false);

			float r = FVector::Distance(boid_transform.GetLocation(), otherboid_transform.GetLocation());
			const float min_dist = 200.0f;
			if (r < min_dist)
			{
				FVector disp = boid_transform.GetLocation() -
					otherboid_transform.GetLocation();
				float dist = disp.Size();
				FVector dir = disp;
				f += dir / dist;
			}
		}

		BoidState[i].TotalForce += Steer(f) * BoidState[i].Weights.Seperate;
	}
#endif
#endif
}


void AFlockEmitter::Cohere(float DeltaTime, float Radius)
{
#if 0
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{ 
		// add force in inverse direction of distance between a and b
		// TODO: Does including its own velocity affect the result of the average?
		FVector f = FVector::ZeroVector;

		FVector boid_loc = GetBoidWorldLocation(BoidMesh[i]);

		for (int32 j = 0; j < BoidMesh.Num(); j++)
		{
			f += GetBoidWorldLocation(BoidMesh[j]);
		}

		f /= BoidMesh.Num();
		DrawDebugSphere(GetWorld(), f, 100.0f, 6, FColor::Magenta);
		//DrawDebugLine(GetWorld(), boid_loc, boid_loc + f, FColor::Magenta, false, -1.0f, 0, 10.0f);
		BoidState[i].TotalForce += Steer(i, (-(boid_loc + f))) * BoidState[i].Weights.Cohere;
	}
#else

	auto get_other_loc {[this](const int32& other) -> FVector
	{
		FTransform boid_transform_other;
		BoidInstanceMesh->GetInstanceTransform(other, boid_transform_other, /*world space?*/true);
		return boid_transform_other.GetLocation();
	}};
	
	auto boid_cohere {[&get_other_loc, this](const int32& boid)
	{
		FVector f = FVector::ZeroVector;

		FTransform boid_transform;
		BoidInstanceMesh->GetInstanceTransform(boid, boid_transform, /*world space?*/true);

		for (int32 j = 0; j < BoidState.Num(); j+= STRIDE)
		{
			f += get_other_loc(j);
			f += get_other_loc(j+1);
			f += get_other_loc(j+2);
			f += get_other_loc(j+3);
			f += get_other_loc(j+4);
			f += get_other_loc(j+5);
			f += get_other_loc(j+6);
			f += get_other_loc(j+7);
		}

		f /= BoidState.Num();
		BoidState[boid].TotalForce += Steer((-(boid_transform.GetLocation() + f))) * BoidState[boid].Weights.Cohere * m_PhysTime;
	}};

	for (int32 i = 0; i < (BoidState.Num()); i += STRIDE)
	{
		boid_cohere(i);
		boid_cohere(i+1);
		boid_cohere(i+2);
		boid_cohere(i+3);
		boid_cohere(i+4);
		boid_cohere(i+5);
		boid_cohere(i+6);
		boid_cohere(i+7);

		#if 0
FVector f = FVector::ZeroVector;
	
		FTransform boid_transform;
		BoidInstanceMesh->GetInstanceTransform(i, boid_transform, /*world space?*/true);

		for (int32 j = 0; j < BoidState.Num(); j++)
		{
			FTransform boid_transform_other;
			BoidInstanceMesh->GetInstanceTransform(j, boid_transform_other, /*world space?*/true);

			f += boid_transform_other.GetLocation();
		}

		f /= BoidMesh.Num();
		BoidState[i].TotalForce += Steer((-(boid_transform.GetLocation() + f))) * BoidState[i].Weights.Cohere;
#endif
	}
#endif
}

void AFlockEmitter::Align(float DeltaTime)
{
#if 0
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		FVector f = FVector::ZeroVector;

		for (int32 j = 0; j < BoidMesh.Num(); j++)
		{
			if (i == j) continue;

			f += BoidState[j].Velocity;
		}

		f /= BoidMesh.Num();
		BoidState[i].TotalForce += Steer(i, GetBoidWorldLocation(BoidMesh[i]) + f) * BoidState[i].Weights.Align;
	}
#else
	
#endif
}

void AFlockEmitter::Scutter(float DeltaTime, float Radius)
{
#if 0
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		const float scutter_r = 500.0f;
		const float scutter_d = 400.0f;
		float range = 0.5f;

		float ScutterSpeed = 3.0f;

		FVector target = BoidState[i].Velocity;
		target.Normalize();
		target *= scutter_d;
		target += GetBoidWorldLocation(BoidMesh[i]);

		float h = BoidState[i].Velocity.HeadingAngle();
		FVector offset = FVector(scutter_r*cos(h + std::sin(FPlatformTime::Seconds() * 10.0f)), scutter_r* std::sin(h + std::sin(FPlatformTime::Seconds() * 10.0f)), 0);
		FVector f = target + offset;

		BoidState[i].TotalForce += Steer(i, f) * BoidState[i].Weights.Scutter;
	}
#else
#endif
}

void AFlockEmitter::Shoot(float DeltaTime, FVector Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot!"));
	
	#if 0
// Raycast from boid to
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		if (!BoidState[i].Target)
		{
			return;
			//~
		}

		const FVector boid_locw = GetBoidWorldLocation(BoidMesh[i]);
		float ray_length = 100.0f;
		FHitResult hit = FHitResult();

		BoidState[i].FireCooldown -= DeltaTime;

		if (BoidState[i].FireCooldown <= 0)
		{
			BoidState[i].bIsFiring = !BoidState[i].bIsFiring;
			BoidState[i].FireCooldown = FMath::RandRange(0.3f, 2.0f);
		}

		if (BoidState[i].bIsFiring)
		{
			GetWorld()->LineTraceSingleByChannel(hit, boid_locw, GetBoidWorldLocation(BoidState[i].Target), ECollisionChannel::ECC_Visibility);
			FVector res_loc = hit.ImpactPoint;

			// TODO: Damage target
			//DrawDebugLine(GetWorld(), boid_locw, res_loc, FColor::Cyan);
			//DrawDebugLine(GetWorld(), boid_locw, m_TargetLocation, FColor::Red);
		}
	}
#endif
}

static float shift = 0.0f;

void AFlockEmitter::FormShape(float DeltaTime, float Radius)
{
	UE_LOG(LogTemp, Warning, TEXT("FormShape!"));

	#if 0
const int32 num_boids = BoidMesh.Num();
	const float diff_angle = (PI*2) / num_boids;
	const float r = 500.0f;
	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		if (!BoidState[i].Target)
		{
			return;
			//~
		}

		float angle = (diff_angle * i) + shift;
		// Chase angle AROUND player
		
		FVector dir = SphericalToCartesian(angle, 0, r);
		dir.Normalize();
		FVector offset = dir * r;
		FVector worldLocation = GetBoidWorldLocation(BoidMesh[i]);
		FVector chase_dir = (GetBoidWorldLocation(BoidState[i].Target) + offset) - worldLocation;
		chase_dir.Normalize();
		BoidState[i].TotalForce += Steer(i, chase_dir) * BoidState[i].Weights.Shape;
	}

	shift += DeltaTime;
#endif
}

void AFlockEmitter::View(float DeltaTime)
{
	#if 0
static float rad = 50.0f;
	static float spacebase = PI/12;
	static uint32 num_casts = 5;

	for (int32 i = 0; i < BoidMesh.Num(); i++)
	{
		UPrimitiveComponent* closest_boid = nullptr;
		float closest_dist = BIG_NUMBER;
		FVector boid_loc = GetBoidWorldLocation(BoidMesh[i]);
		FVector boid_vel = BoidState[i].Velocity;
		FVector basedir  = BoidMesh[i]->GetForwardVector();
		float base_theta = FMath::Atan2(basedir.Y, basedir.X);
		float base_phi = FMath::Atan2(basedir.Z, basedir.Y);
		num_casts += num_casts + 1 & 1;
		int8 casts_range = num_casts / 2;
		float space = spacebase / num_casts;

		for (int32 j = -casts_range; j <= casts_range; j++)
		{
			float angle = base_theta + (j*space);
			float phi = base_phi + (j*space);

			FVector dir = SphericalToCartesian(angle, phi, rad);

			//make raycast cone

			dir *= rad;
			FHitResult hit;
			GetWorld()->LineTraceSingleByChannel(hit, boid_loc, boid_loc + dir, ECollisionChannel::ECC_GameTraceChannel4);
			//DrawDebugLine(GetWorld(), boid_loc, boid_loc + dir, FColor::Magenta, false, -1.0f, 0, 5.0f);

			if (hit.Component.Get())
			{
				if (hit.Component->ComponentTags.Num() > 0 && hit.Component->ComponentTags[0].ToString() == TEXT("boid"))
				{
					uint8 opp = std::atoi((const char*)(*hit.Component->ComponentTags[1].ToString()));
					uint8 player = (uint8)BoidState[i].bIsPlayer;
					
					if (opp ^ player)
					{
						bool bSetClosest = false;
						FVector cur_disp = GetBoidWorldLocation(BoidMesh[i]) - GetBoidWorldLocation(hit.Component.Get());
						float cur_dist = cur_disp.Size();

						// Add to target array
						if (!closest_boid || cur_dist < closest_dist)
						{
							bSetClosest = !bSetClosest;
						}

						if (bSetClosest)
						{
							closest_dist = cur_dist;
							closest_boid = hit.Component.Get();
						}
					}
				}
			}
		}

		// Set current target, can be null if no potential targets in sight	
		BoidState[i].Target = closest_boid;
	}
#endif
}

FVector AFlockEmitter::GetBoidWorldLocation(UPrimitiveComponent* Boid)
{
	const FVector loc = Boid->GetOwner()->GetActorLocation();
	const FVector boid_loc = Boid->GetRelativeTransform().GetLocation();
	return (loc + boid_loc);
}

FVector AFlockEmitter::Steer(const FVector& Dir) const
{
	FVector dir = ((Dir.GetSafeNormal()) * MaxSpeed * m_PhysTime);

	float vecSize = dir.Size();
	vecSize = FMath::Clamp(vecSize, 2.0f, 10.0f);

	return (dir * vecSize * m_PhysTime);
}

// Called every frame
void AFlockEmitter::Tick(float DeltaTime)
{
	m_PhysTime += DeltaTime;
	DrawDebugSphere(GetWorld(), FVector::ZeroVector, 40.0f, 8, FColor::White);	

	Super::Tick(DeltaTime);

#if 0
	View(DeltaTime);
#endif
	
	while (m_PhysTime >= m_PhysTimeStep)
	{
		BIT_SWITCH((uint32)BoidBehaviour)
		{
		case (uint32)EEnemyBehavaiour::eNavigate:
			//Chase(DeltaTime, 0.0f);
			Separate(DeltaTime, 0.0f);
			//Align(DeltaTime);
			Cohere(DeltaTime, 0.0f);
			//Scutter(DeltaTime, 0.0f);
			//FormShape(DeltaTime, 0.0f);
			break;
		case (uint32)EEnemyBehavaiour::eShoot:
			Shoot(DeltaTime, FVector::ZeroVector);
			break;
		case (uint32)EEnemyBehavaiour::eFormCircle:
			break;
		default:
			break;
		}

	#if 0
		for (int32 i = 0; i < BoidMesh.Num(); i++)
		{
			// integrate
			FVector totalForce = BoidState[i].TotalForce;
			totalForce = totalForce.GetClampedToSize(0.1f, 1.0f);
			BoidState[i].Velocity += totalForce;
			BoidState[i].TotalForce = FVector::ZeroVector;
			const float drag = 1/* * BaseDrag / BoidMass */;
			const float speed = BaseSpeed * BoidMass * 0.001f;
			FVector worldLocation = GetBoidWorldLocation(BoidMesh[i]);
			FVector forward = BoidMesh[i]->GetForwardVector();
			FVector right = BoidMesh[i]->GetRightVector();
			FVector up = BoidMesh[i]->GetUpVector();
			FVector vel_unit = BoidState[i].Velocity.GetSafeNormal();

			BoidMesh[i]->AddRelativeLocation(BoidState[i].Velocity * speed);
			FVector world_loc = GetBoidWorldLocation(BoidMesh[i]);
			FVector dir = BoidState[i].Velocity;
		
			FRotator boid_rot = UKismetMathLibrary::MakeRotFromX(vel_unit);
			BoidMesh[i]->SetWorldRotation(boid_rot);

			// Print debug info 
	#if 0
			FString name = BoidMesh[i]->GetName();
			FString location = BoidMesh[i]->GetRelativeTransform().GetLocation().ToString();
			FVector worldPos = GetActorLocation() + BoidMesh[i]->GetRelativeTransform().GetLocation();
			FString locationWorld = worldPos.ToString();
			FString vel = BoidState[i].Velocity.ToString();

			DrawDebugLine(GetWorld(), worldLocation, worldLocation + (forward * 100.0f), FColor::Red, false, -1.0f, (uint8)'\000', 3.0f);
			DrawDebugLine(GetWorld(), worldLocation, worldLocation + (right * 100.0f), FColor::Green, false, -1.0f, (uint8)'\000', 3.0f);
			DrawDebugLine(GetWorld(), worldLocation, worldLocation + (up * 100.0f), FColor::Blue, false, -1.0f, (uint8)'\000', 3.0f); */

			QUICK_LOG_UPDATE("Name: " + name + " P: " + location + " WP: " + locationWorld + " V: " + vel);
	#endif
		}
	#else
	#if 1

		auto integrate_forces{[this](const int32& boid)
		{
			FTransform boid_transform;
			BoidInstanceMesh->GetInstanceTransform(boid, boid_transform, /*world space?*/true);

			FVector totalForce = BoidState[boid].TotalForce;
			totalForce = totalForce.GetClampedToSize(0.1f, 1.0f);
			BoidState[boid].Velocity += totalForce;
			BoidState[boid].TotalForce = FVector::ZeroVector;
			const float drag = 1/* * BaseDrag / BoidMass */;
			const float speed = BaseSpeed * BoidMass;
			FVector vel_unit = BoidState[boid].Velocity.GetSafeNormal();

			boid_transform.SetLocation(boid_transform.GetLocation() + (BoidState[boid].Velocity * speed * m_PhysTime));
			FRotator boid_rot = UKismetMathLibrary::MakeRotFromX(vel_unit);
			boid_transform.SetRotation(boid_rot.Quaternion());
			BoidInstanceMesh->UpdateInstanceTransform(boid, boid_transform, true);
		}};

		for (int32 i = 0; i < BoidState.Num(); i+= STRIDE)
		{
			integrate_forces(i);
			integrate_forces(i+1);
			integrate_forces(i+2);
			integrate_forces(i+3);
			integrate_forces(i+4);
			integrate_forces(i+5);
			integrate_forces(i+6);
			integrate_forces(i+7);
		}

		m_PhysTime -= m_PhysTimeStep;

	#endif
	#endif
	}
}

