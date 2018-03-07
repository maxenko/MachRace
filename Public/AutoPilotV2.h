// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoPilotV2.generated.h"

UENUM(BlueprintType)
enum class AutopilotPathStatus : uint8 {
	NoPath			UMETA(DisplayName = "NoPath"),	// completely blocked
	Path	 		UMETA(DisplayName = "Path"),	// obstacles on the way, but need to go around them
	Clear			UMETA(DisplayName = "Clear"),	// no obstacles whatsoever
};

UENUM(BlueprintType)
enum class AutopilotStatus : uint8 {
	Chasing 		UMETA(DisplayName = "Chasing"),
	Manuvering 		UMETA(DisplayName = "Manuvering"),
	Idle 			UMETA(DisplayName = "Idle"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNavigate, FVector, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObstaclesDetected, TArray<FHitResult>, ObstacleHitLocations);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusChange, AutopilotStatus, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathStatusChange, AutopilotPathStatus, PathStatus);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutoPilotV2 : public UActorComponent {

	GENERATED_BODY()

public:	
	UAutoPilotV2();

protected:


	FVector ownerLoc = FVector::ZeroVector;
	USceneComponent* root = NULL;
	int32 scanWidth = 0;
	bool scanIsRunning = false;
	bool scanInitiated = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;

	TArray<FHitResult> sphereTrace(FVector from, FVector to, float sphereRadius, TArray<AActor*> ignoredActors);
	FVector calculateNavigationDirection(TArray<FHitResult> blockingHits);

	void resetScan();
	int32 findPath_singleSphereTrace();
	int32 findPath_doubleSphereTrace(bool &pathFound);

	FVector calculateYAlignmentVelocity(FVector destination);

	bool shouldChase();
	bool isStableY();

	void chase();
	void manuver();

	float delta = 0.f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//////////////////////////////////////////////////////////////////////////
	// Debug
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool ShowDebugExtra = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	float DebugHitPointSize = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	FColor DebugHitPointColor = FColor::Orange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Debug")
	FColor DebugAverageHitPointColor = FColor::Blue;


	//////////////////////////////////////////////////////////////////////////
	// Navigation
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	UStaticMeshComponent* OwnerPhysicsComponent = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System|Navigation")
	AutopilotStatus Status = AutopilotStatus::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System|Navigation")
	AutopilotPathStatus PathStatus = AutopilotPathStatus::NoPath;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System|Navigation")
	FOnStatusChange OnStatusChange;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System|Navigation")
	FOnPathStatusChange OnPathStatusChange;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System|Navigation")
	FOnObstaclesDetected OnObstaclesDetected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	bool NavigateEachTick = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	bool ChaseTarget = true;

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
	float MaximumManuveringSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ManuveringDecayRadius = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ManuveringAdjustmentSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ScanDistance = 5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float AlignmentThreshold = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float StabilizationSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<TEnumAsByte<EObjectTypeQuery>> DetectableObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<AActor*> IgnoreList;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	FVector NavigateDirection = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find Path", Keywords = "Find clear path in given direction. Triggers OnObstaclesDetected when obstacles are found. Returns true when path is found, false otherwise. Should only run once per tick."), Category = "MachRace|Gameplay|Navigation")
	bool FindPath();

	FTimerHandle ScanTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ScanInterval = .2f;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan", Keywords = "Runs a scan sequence."), Category = "MachRace|Gameplay|Navigation")
	void RunScanSequence();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Desired Velocity", Keywords = "Runs a scan sequence."), Category = "MachRace|Gameplay|Navigation")
	FVector CalculateDesiredVelocity();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Navigate", Keywords = "Umbrella navigation method. Either dodges objects or follows target."), Category = "MachRace|Gameplay|Navigation")
	void Navigate();


	//////////////////////////////////////////////////////////////////////////
	// Gameplay
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector TargetOffset = FVector::ZeroVector;
};
