// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoPilotV2.generated.h"

UENUM(BlueprintType)
enum class AutopilotStatus : uint8 {
	Blocked			UMETA(DisplayName = "Blocked"),
	Clear 			UMETA(DisplayName = "Clear"),
	Dodging 		UMETA(DisplayName = "Dodging"),
	Chasing 		UMETA(DisplayName = "ChasingTarget"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNavigate, FVector, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObstaclesDetected, TArray<FVector>, ObstacleHitLocations);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutoPilotV2 : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAutoPilotV2();

protected:
	virtual void BeginPlay() override;
	TArray<FHitResult> sphereTrace(FVector from, FVector to, float sphereRadius, TArray<AActor*> ignoredActors);
	FVector calculateNavigationDirection(TArray<FHitResult> blockingHits);
	FVector ownerLoc = FVector::ZeroVector;
	USceneComponent* root = NULL;
	int32 scanWidth = 0;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//////////////////////////////////////////////////////////////////////////
	// Debug
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	float DebugHitPointSize = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	FColor DebugHitPointColor = FColor::Orange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	FColor DebugAverageHitPointColor = FColor::Blue;


	//////////////////////////////////////////////////////////////////////////
	// Navigation
	//////////////////////////////////////////////////////////////////////////

	AutopilotStatus Status = AutopilotStatus::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ScanRadius = 200.0;
	
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	TArray<FVector> DetectedObstacleHits;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	float ClosestHitWeight = 0.0;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	FVector ClearPathVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ManuveringAccelerationMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ScanDistance = 5000;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System|Navigation")
	FOnObstaclesDetected OnObstaclesDetected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<TEnumAsByte<EObjectTypeQuery>> DetectableObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<AActor*> IgnoreList;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	FVector NavigateDirection = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan", Keywords = "Scan around the actor, to find obstacles. Triggers OnObstaclesDetected when obstacles are found."), Category = "MachRace|Gameplay|Navigation")
	bool Scan();

};
