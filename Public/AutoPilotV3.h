// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoPilotV3.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutoPilotV3 : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoPilotV3();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float delta = 0.0;

	float getVelocityX(float targetSpeed, float ownerSpeed, FVector targetLoc, FVector ownerLoc);
	float getVelocityY(float ownerSpeed, FVector targetLoc, FVector ownerLoc);
	float getVelocityZ(float ownerSpeed, FVector targetLoc, FVector ownerLoc);

	FVector getFinalVelocity();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Target actor to follow (in world space)."))
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Navigation", meta = (ToolTip = "Offset at which to follow the target."))
	FVector TargetOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Navigation", meta = (ToolTip = "Owner actor's physics component AutoPilot will use."))
	UStaticMeshComponent* OwnerPhysicsComponent = NULL;

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
};
