// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "RaceDroneBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API ARaceDroneBase : public ARaceActorBase
{
	GENERATED_BODY()
		
private:

	// generate scan field intervals
	TArray<float> getIntervals();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	AActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector FollowOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DodgeObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 DetectionRays = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DetectionRayInterval = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float ScanDistance = 3000.0f;
	
	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay")
	TArray<FVector> DetectObstacles();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
};
