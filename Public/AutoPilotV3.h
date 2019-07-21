// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.h"
#include "Components/ActorComponent.h"

#include "MachRace.h"
#include "X.h"
#include "DrawDebugHelpers.h"

#include "AutoPilotV3.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathConditionChange, AutopilotPathCondition, Status);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutoPilotV3 : public UActorComponent {
	GENERATED_BODY()


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float delta = 0.0;

	float getVelocityX(float targetSpeed, float ownerSpeed, FVector targetLoc, FVector ownerLoc);

	float getFollowTargetVelocityY(float ownerSpeed, FVector targetLoc, FVector ownerLoc);
	float getDodgeVelocityY(float ownerSpeed, FVector ownerLoc);

	float getVelocityZ(float ownerSpeed, FVector targetLoc, FVector ownerLoc);

	FVector getFinalVelocity();

	TArray<FHitResult> sphereTrace(FVector from, FVector to, float sphereRadius, TArray<AActor*> ignoredActors);
	TArray<FHitResult> filterHits(TArray<FHitResult> hits);

	bool doubleSphereTrace(FVector from, FVector to, float scanRadius, TArray<AActor*> ignoredActors, FVector& clearScanLocation);

	int32 scanWidth = 1;
	bool scheduleDoubleSphereTraceNextTick = false;

public:	

	// Sets default values for this component's properties
	UAutoPilotV3();


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

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//////////////////////////////////////////////////////////////////////////
	// Setup
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Target actor to follow (in world space)."))
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Owner actor's physics component AutoPilot will use."))
	UStaticMeshComponent* OwnerPhysicsComponent = NULL;


	//////////////////////////////////////////////////////////////////////////
	// Gameplay
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Offset at which to follow the target."))
	FVector TargetOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Animation", meta = (ToolTip = "Bob range, Z axis only."))
	float BobRangeZ = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Animation", meta = (ToolTip = "Current Bob target value."))
	float CurrentBobZ = 0;


	//////////////////////////////////////////////////////////////////////////
	// Velocity adjustment
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Speed at which velocity X is adjusted (Finterp)."))
	float VelocityAdjustmentSpeedX = 15.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Speed at which velocity Y is adjusted (Finterp)."))
	float VelocityAdjustmentSpeedY = 15.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Speed at which velocity Z is adjusted (Finterp)."))
	float VelocityAdjustmentSpeedZ = 15.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Percentage of target speed, at which AutoPilot will accelerate/decelerate in X."))
	float MaxAccelerationXPct = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Max additional speed at which AutoPilot will accelerate to position itself in Y."))
	float MaxAccelerationY = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Max additional speed at which AutoPilot will accelerate to position itself in Z."))
	float MaxAccelerationZ = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Threshold distance within which AutoPilot will start to slow down."))
	float AccelerationXThreshold = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Threshold distance within which AutoPilot will start to slow down."))
	float AccelerationYThreshold = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Threshold distance within which AutoPilot will start to slow down."))
	float AccelerationZThreshold = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Wether or not autopilot will attempt to align with the Target+TargetOffset in Y axis."))
	bool AlignWithTargetInY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Wether or not autopilot will attempt to align with the Target+TargetOffset in Z axis."))
	bool AlignWithTargetInZ = true;


	//////////////////////////////////////////////////////////////////////////
	// Scanning / Tracing
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay|Navigation", meta = (DisplayName = "Find Path", Keywords = "Find clear path in given direction. Triggers OnObstaclesDetected when obstacles are found. Returns true when path is found, false otherwise. Should only run once per tick."))
	void Scan(FVector from, FVector to, float scanRadius, TArray<AActor*> ignoredActors);

	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay|Navigation", meta = (DisplayName = "Find Path", Keywords = "Runs the scan when ScanAutomatically is enabled."))
	void AutoScan();

	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay|Navigation", meta = (DisplayName = "Flip CurrentBobZ", Keywords = "Flip CurrentBobZ to the other extreme."))
	void FlipBobZTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<AActor*> IgnoreList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "If enabled - component will run a scan every tick. If disabled, scan is not ran automatically, but can be performed manually."))
	bool ScanAutomatically = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation")
	TArray<TEnumAsByte<EObjectTypeQuery>> DetectableObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (ToolTip = "Actors and components that have these tags - will be filitered out from detection by autopilot."))
	TArray<FName> TagsToIgnoreDuringPathFinding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Scan ahead X distance."))
	float ScanDistanceX = -7000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Radius of the sphere trace forward."))
	float DefaultScanRadius = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Last scan location clear of obstacles."))
	FVector LastClearPathScanLoc = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	AutopilotPathCondition PathCondition = AutopilotPathCondition::Clear;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Navigation")
	AutopilotIntent Intent = AutopilotIntent::ResumeNonDodgeBehavior;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|System|Navigation")
	FOnPathConditionChange OnPathConditionChange;
};