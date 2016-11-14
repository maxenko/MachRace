// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "FlightNavigator.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObstacleFoundAhead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObstacleFoundSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearAhead);


USTRUCT(BlueprintType)
struct FFlightNavigationRay {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector To = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Distance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FHitResult Hit;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UFlightNavigator : public UActorComponent
{
	GENERATED_BODY()

private:

	// generate scan field intervals
	TArray<float> getIntervals();
	TArray<FFlightNavigationRay> getForwardScan();
	bool hasSideObstacle(int32 dir); // -1/1 for direction
	FFlightNavigationRay noHitRay;
	void drawDebug(TArray<FFlightNavigationRay> rays);
	TArray<FFlightNavigationRay> generateNoHitResult();
	void dodge(float delta);
	void followTarget();
	void moveInFrontOfTarget(float delta, FVector& aggregate);

	bool hasObstacle = false;
	bool isDecayingY = true;

	FVector aggregateWorldLocation;

	// physics ops
	void nudge(EAxisList::Type axis, FVector from, FVector to);
	void decay(EAxisList::Type axis, float delta);

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool FollowTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool MoveInFrontOfTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MoveInTargetVelocity = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector FollowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DodgeObstacles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float DodgeSpeedDecay = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 DetectionRays = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 SideDetectionRays = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DetectionRayInterval = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float FollowDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float DodgeSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float ScanDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float LateralScanDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool DrawDebug = true;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FObstacleFoundAhead ObstacleFoundAhead;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FObstacleFoundAhead ObstacleFoundSide;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FObstacleFoundAhead ClearAhead;

	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay")
	FVector GetToLoc();

	// Sets default values for this component's properties
	UFlightNavigator();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

};
