// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JingleJamSandwichGameModeBase.h"
#include <Components/BoxComponent.h>
#include "Toy.generated.h"

UCLASS()
class JINGLEJAMSANDWICH_API AToy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void RandomiseToy();

	void LoadMesh(FString MeshName);

	float MovementSpeed = 100.0f;

	UBoxComponent* BoxComponent;

	UStaticMeshComponent* mesh;
	FString materialType;
	int32 itemType = 0;
	EMachineColour colour = eColourMax;
};
