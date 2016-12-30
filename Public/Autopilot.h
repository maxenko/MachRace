// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "EnvironmentScanner.h"
#include "Autopilot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAngularVelocityRestoredToZero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlignmentWithinThreshold);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutopilot : public UActorComponent
{
	GENERATED_BODY()

private:

	float lastDelta = 0.0;
	FVector getTargetVelocity(); // calculates target velocity (as in velocity toward which autopilot interps to)

	int scanAroundStale = true;
	TArray<FHitResult> scanAroundHits;
	FVector obstacleLoc = FVector::ZeroVector;
	TArray<FFlightNavigationRay> scanAheadHits;
	TArray<FFlightNavigationRay> scanSidesHits;

	bool goingLeft = false;
	bool goingRight = false;

	void decayRadialVelocity();


public:	
	// Sets default values for this component's properties
	UAutopilot();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ScanAhead", Keywords = "Scans in front of the owner."), Category = "MachRace|Gameplay")
	void ScanAhead();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ScanAround", Keywords = "Scans around the owner."), Category = "MachRace|Gameplay")
	void ScanAround();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ScanSides", Keywords = "Scans to the side of the owner."), Category = "MachRace|Gameplay")
	void ScanSides();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Velocity", Keywords = "Interpolates velocity once from current to target velocity."), Category = "MachRace|Gameplay")
	void UpdateVelocity();

	
	//////////////////////////////////////////////////////////////////////////
	// Gameplay events / indicators
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FAngularVelocityRestoredToZero OnAngularVelocityRestoredToZero;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FAlignmentWithinThreshold OnAlignmentWithinThreshold;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool ObstacleDetected = false; // front (-X)

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool ObstacleLeftDetected = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool ObstacleRightDetected = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector SafeSpaceVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector DodgeVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector AlignWithTargetVelocity = FVector::ZeroVector;


	//////////////////////////////////////////////////////////////////////////
	// Scanners
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	UEnvironmentScanner* ForwardScanner = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	UEnvironmentScanner* SideScanner = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<TEnumAsByte<EObjectTypeQuery>> ScanAroundObjectTypes;


	//////////////////////////////////////////////////////////////////////////
	// Internals mechanics / runtime switches / properties
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FTransform DefaultTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool RestoreAngularVelocityToZero = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool RestoreRotationToDefault = false;


	//////////////////////////////////////////////////////////////////////////
	// Settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool FollowTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool AlignWithTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Dev|Debug")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxFollowSpeed = -2000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0") )
	float MaxSelfDistancingSpeed = 200.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0"))
	float MaxDodgeVelocity = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0"))
	float SelfDistanceTo = 350.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float FollowOffset = -1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float AlignWithTargetSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float AlignWithTargetSpeedFaloff = 200;

	// not velocity of the target, but rather desired velocity that autopilot adjusts to with every UpdateVelocity()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector TargetVelocity; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float VelocityChangeSpeed = 5.0;

};