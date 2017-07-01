// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnActorWhenInVisionConeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVisionSpawnTrigger);

UCLASS()
class MACHRACE_API ASpawnActorWhenInVisionConeBase : public AActor
{
	GENERATED_BODY()

	
	
public:	
	// Sets default values for this actor's properties
	ASpawnActorWhenInVisionConeBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	APlayerCameraManager* CameraManager = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float TriggerDistance = 10000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float TriggerConeAngle = 90.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool TriggerContinuously = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	int32 TriggerCount = 0;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System")
	FOnVisionSpawnTrigger OnTrigger;
	
};
