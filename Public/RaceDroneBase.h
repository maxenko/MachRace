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
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	AActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector FollowOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DodgeObstacles;

	
	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay")
	TArray<FVector> DetectObstacles(FVector direction);
	
};
