// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommonTypes.h"
#include "AutoPilotV2.generated.h"


UENUM(BlueprintType)
enum class AutopilotStatus : uint8 {
	Chasing 		UMETA(DisplayName = "Chasing"),
	Manuvering 		UMETA(DisplayName = "Maneuvering"),
	Idle 			UMETA(DisplayName = "Idle"),
};

UENUM(BlueprintType)
enum class AutopilotChasingStatus : uint8 {
	NotAlignedWithTarget 		UMETA(DisplayName = "Not Aligned With Target"),
	AlignedWithTarget 			UMETA(DisplayName = "Aligned With Target"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNavigate, FVector, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObstaclesDetected, TArray<FHitResult>, ObstacleHitLocations);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusChange, AutopilotStatus, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathStatusChange, AutopilotPathStatus, PathStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOrientationEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutoPilotV2 : public UActorComponent {

	GENERATED_BODY()

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
	TArray<FHitResult> filterHits(TArray<FHitResult> hits);
	int32 findPath_singleSphereTrace();
	int32 findPath_doubleSphereTrace(bool &pathFound);
	bool sideIsBlocked(Side side);

	FVector calculateYAlignmentVelocity(FVector destination);

	bool shouldChase();

	FVector calcChaseVelocity();
	FVector calcManuverVelocity();

	void setAngularImpulseAndRotationFlags();

	FVector decayAngularVelocity();

	Side pathYSide(FVector path);

	float delta = 0.f;

	bool wasJustHit = false;
	bool decayAngularVelocityToZero = false;
	bool restoreRotationToVisualOrientation = false;

	bool hasBroadcast_CanRestoreOrientation = false;

	UFUNCTION()
	void onOwnerHit();

public:	

	UAutoPilotV2();

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
	AActor* Target = NULL;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Wether or not Navigation algorithm should run every tick. If false it will not run at all, unless manually called."))
	bool NavigateEachTick = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Radius of the sphere traces used in scans."))
	float ScanRadius = 200.0;
	
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	TArray<FVector> DetectedObstacleHits;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation", meta = (ToolTip = "What autopilot things is a clear path to follow. This is end if the sphere trace."))
	FVector ClearPathVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<TEnumAsByte<EObjectTypeQuery>> DetectableObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<AActor*> IgnoreList;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	FVector NavigateDirection = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find Path", Keywords = "Find clear path in given direction. Triggers OnObstaclesDetected when obstacles are found. Returns true when path is found, false otherwise. Should only run once per tick."), Category = "MachRace|Gameplay|Navigation")
	bool FindPath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float ScanInterval = .2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float SideScanInterval = .3f;

	FTimerHandle ScanTimerHandle;

	FTimerHandle SideScanTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	float SideScanDistance = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	FVector SideScanBoxExtent = FVector(1000, 100, 100);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan", Keywords = "Runs a scan sequence."), Category = "MachRace|System|Navigation")
	void RunScanSequence();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Ambient Velocity", Keywords = "Calculates other velocities in addition to manuver or chase velocity."), Category = "MachRace|System|Navigation")
	FVector CalculateAmbientVelocity();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Navigate", Keywords = "Umbrella navigation method. Either dodges objects or follows target."), Category = "MachRace|System|Navigation")
	void Navigate();


	//////////////////////////////////////////////////////////////////////////
	// Gameplay
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Normally speed is adjusted by distance to the desired Y component, but it can be multiplied with this."))
	float ManuveringAccelerationMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Maximum physical force in Y axis at which autopilot will dodge obstacles."))
	float MaximumManuveringPhysicsForce = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Distance at which autopilot starts to decay speed during a manuver."))
	float ManuveringDecayRadius = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Speed at which autopilot will manuver into place (vinterp)."))
	float ManuveringInterpSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Speed at which autopilot will manuver into place in Y-axis (vinterp)."))
	float ManuveringInterpSpeedY = ManuveringInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "How far sphere traces are cast in X axis. Negative or positive."))
	float ScanDistance = -5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Distance at which autopilot will consider itself aligned. This is used to make decisions during dodging."))
	float AlignmentThreshold = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Actors and components that have these tags - will be filitered out from detection by autopilot."))
	TArray<FName> TagsToIgnoreDuringPathFinding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DontFollowInY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector TargetFollowOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector TargetChaseOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Factor of Target speed by which autopilot can accelerate to catch up with the target."))
	float FollowTargetAccelerationFactor = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Distance at which follow acceleration will start to decay."))
	float FollowTargetAccelerationDecayDistance = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Distance in Y axis at which autopilot will think its aligned with Target (if its chasing)."))
	float AlignedWithTargetDist = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Follow adjustment speed (VInterp)."))
	float FollowSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Maximum speed at which autopilot will attempt to follow Target, including speeding to catch up. This is essentially a clamp on total speed."))
	float MaxFollowPhysicsSpeed = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Maximum speed used to catch up in addition to Target velocity."))
	float MaxAdditionalAccelerationPhysicsSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Wether or not Target should be chased (if its set). Chasing mean following target in X axis."))
	bool ChaseTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Wether or not Target should be followed (if its set). Following means flying at the same speed as the target with TargetFollowOffset offset."))
	bool FollowTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Not Implemented", meta = (ToolTip = "Range of bob travel - in Z."))
	FVector2D BobRange = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Not Implemented", meta = (ToolTip = "Speed of bob travel."))
	float BobSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Not Implemented")
	FVector FlyAwayOffset = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MachRace|Not Implemented", Keywords = "Flys away in the direction specified by FlyAwayOffset."), Category = "MachRace|System|Navigation")
	void FlyAway() {}


	//////////////////////////////////////////////////////////////////////////
	// Presentation
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation", meta = (ToolTip = "Where should "))
	FRotator VisualOrientation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Presentation")
	FOnOrientationEvent OnCanRestoreOrientation;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Presentation")
	FOnOrientationEvent OnOrientationRestored;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Proximity at which restore will consider itself complete."))
	float RestoreVisualOrientationNearTo = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Speed at which Owner will stop spinning if it is spinning due to angular velocity."))
	float DecayAngularVelocitySpeed = 1;
};
