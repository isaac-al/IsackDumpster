// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Krampus.generated.h"

class UCapsuleComponent;

UCLASS()
class JINGLEJAMSANDWICH_API AKrampus : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKrampus();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite)
		USkeletalMeshComponent* mesh;
	UPROPERTY(BlueprintReadWrite)
		USkeleton* skeleton = nullptr;
	UPROPERTY(BlueprintReadWrite)
		UPhysicsAsset* physicsAsset = nullptr;

	UPROPERTY(BlueprintReadWrite)
		UCapsuleComponent* CapsuleComp;

	UFUNCTION(BlueprintCallable)
		void LoadMesh();

	FVector Velocity;
	float SpeedModifier = 2500.0f;

	void MoveX(float amount);
	void MoveY(float amount);
};