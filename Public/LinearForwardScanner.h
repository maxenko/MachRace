// Copyright 2015 - Max Enko

#pragma once

#include "EnvironmentScanner.h"
#include "CommonTypes.h"
#include "LinearForwardScanner.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MACHRACE_API ULinearForwardScanner : public UEnvironmentScanner
{
	GENERATED_BODY()

private:
	
	TArray<float> getIntervals();
	TArray<FFlightNavigationRay> getForwardScan();
	
public:

	TArray<FFlightNavigationRay> Scan() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System", meta = (UIMin = "0", ClampMin = "0"))
	int32 DetectionRays = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DetectionRayInterval = 20.0f;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool ObstacleDetected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0"))
	float ScanDistance = 3000.0f;

};
