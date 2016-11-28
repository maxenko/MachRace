// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
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
	void keepUpWithTarget();
	FVector getTargetVelocity(); // calculates target velocity (as in velocity toward which autopilot interps to)

public:	
	// Sets default values for this component's properties
	UAutopilot();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FollowTarget", Keywords = "Follow target, accelerating and decelerating as necessary."), Category = "MachRace|Gameplay")
	void AdjustVelocityToFollowTarget(float delta);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan", Keywords = "Scans around the owner."), Category = "MachRace|Gameplay")
	void Scan();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Velocity", Keywords = "Interpolates velocity once from current to target velocity."), Category = "MachRace|Gameplay")
	void UpdateVelocity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float NavigationSpeed = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool FollowTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxFollowSpeed = -2000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float FollowOffset = -1000.0;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	UCurveFloat* FollowOffsetApproachCurve;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector DodgeVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector TargetFollowVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	FVector AlignWithTargetVelocity = FVector::ZeroVector;

	// not velocity of the target, but rather desired velocity that autopilot adjusts to with every UpdateVelocity()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector TargetVelocity; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float VelocityChangeSpeed = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;
	
};