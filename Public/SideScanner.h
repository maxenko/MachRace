// Copyright 2015 - Max Enko

#pragma once

#include "EnvironmentScanner.h"
#include "SideScanner.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MACHRACE_API USideScanner : public UEnvironmentScanner
{
	GENERATED_BODY()

private:
	TArray<FHitResult> hits;
	TArray<FFlightNavigationRay> emptyResult;

public:
	TArray<FFlightNavigationRay> Scan() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float SideScanDist = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector ScanBoxExtent = FVector(100, 100, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector ScanOriginOffset = FVector(-300, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool ObstacleOnTheLeft = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool ObstacleOnTheRight = false;
	
};
