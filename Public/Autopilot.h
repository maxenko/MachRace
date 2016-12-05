// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "EnvironmentScanner.h"
#include "Autopilot.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EDroneOperationType : uint8
{
	DroneOp_FollowTarget 			UMETA(DisplayName = "Follow Target"),
	DroneOp_Dodge					UMETA(DisplayName = "Dodge Obstacles"),
	DroneOp_AlignWithTarget			UMETA(DisplayName = "Get in front of Player")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutopilot : public UActorComponent
{
	GENERATED_BODY()

private:

	float lastDelta = 0.0;
	FVector getTargetVelocity(); // calculates target velocity (as in velocity toward which autopilot interps to)

	int scanAroundStale = true;
	TArray<FHitResult> scanAroundHits;
	TArray<FHitResult> scanAheadHits;

public:	
	// Sets default values for this component's properties
	UAutopilot();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FollowTarget", Keywords = "Follow target, accelerating and decelerating as necessary."), Category = "MachRace|Gameplay")
	void AdjustVelocityToFollowTarget(float delta);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ScanAhead", Keywords = "Scans in front of the owner."), Category = "MachRace|Gameplay")
	void ScanAhead();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ScanAround", Keywords = "Scans around the owner."), Category = "MachRace|Gameplay")
	void ScanAround();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Velocity", Keywords = "Interpolates velocity once from current to target velocity."), Category = "MachRace|Gameplay")
	void UpdateVelocity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	UEnvironmentScanner* ForwardScanner = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ClearanceWidth = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float NavigationSpeed = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool FollowTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool AlignWithTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxFollowSpeed = -2000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0") )
	float MaxSelfDistancingSpeed = 200.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay", meta = (UIMin = "0", ClampMin = "0"))
	float SelfDistanceTo = 350.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float FollowOffset = -1000.0;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	UCurveFloat* FollowOffsetApproachCurve;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector SafeSpaceVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector DodgeVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float AlignWithTargetSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float AlignWithTargetSpeedFaloff = 200;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector AlignWithTargetVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool ObstacleDetected = false;

	// not velocity of the target, but rather desired velocity that autopilot adjusts to with every UpdateVelocity()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector TargetVelocity; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float VelocityChangeSpeed = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<TEnumAsByte<EObjectTypeQuery>> ScanAroundObjectTypes;

};