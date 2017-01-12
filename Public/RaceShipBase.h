// Copyright 2015 - Max Enko

#pragma once

#include "ShipBase.h"
#include "RaceShipBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccelerate, float, SpeedIncrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecelerate, float, SpeedDecrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMachSpeedChange, int32, MachSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBank, int32, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIgnition, bool, OnOff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccelerationFlash, float, Multiplier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecelerationFlash, float, Multiplier);

UCLASS()
class MACHRACE_API ARaceShipBase : public AShipBase {
	GENERATED_BODY()

private:
	UPrimitiveComponent* getRootAsPrimitive(bool& success);
	void decayLateralMovement(float delta);
	void decayRotationToZero(float delta);
	int32 previousMach = 0;
	void changeSpeed(float by);

public:

	ARaceShipBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Accelerate", Keywords = "Accelerate the ship."), Category = "MachRace|Gameplay")
	void Accelerate(float forwardVelocity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Decelerate", Keywords = "Decelerate the ship."), Category = "MachRace|Gameplay")
	void Decelerate(float forwardVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MinDistFromGround = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float GroundFollowSpeed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYaw = 4.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYawSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool EnableLateralDecay = false;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnAccelerate OnAccelerate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnDecelerate OnDecelerate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnAccelerationFlash OnAccelerateFlash;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnDecelerationFlash OnDecelerationFlash;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnMachSpeedChange OnMachSpeedChange;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Presentation")
	FOnIgnition OnIgnition;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnBank OnBank;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Trigger Ignition", Keywords = "Starts ignition sequence, purely cosmetic."), Category = "MachRace|Presentation")
	void TriggerIgnition(bool onOff);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Ground Dist", Keywords = "Check and record gorund distance to history."), Category = "MachRace|System")
	float CheckGroundDist(FHitResult& hit);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bank", Keywords = "Banks ship."), Category = "MachRace|System")
	void Bank(FVector impulse);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spin", Keywords = "Spin ship."), Category = "MachRace|System")
	FVector Spin(FVector impulse, float maxX = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Yaw", Keywords = "Yaw ship."), Category = "MachRace|System")
	float Yaw(bool direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Speed", Keywords = "Gets theoretical ship speed."), Category = "MachRace|System")
	float GetTheoreticalSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Actual Speed", Keywords = "Gets actual ship speed."), Category = "MachRace|System")
	float GetSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Lateral Speed", Keywords = "Gets theoretical lateral ship speed."), Category = "MachRace|System")
	float GetTheoreticalBankingSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Actual Lateral Speed", Keywords = "Gets actual lateral ship speed."), Category = "MachRace|System")
	float GetBankingSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Ships Angular Impulse.", Keywords = "Gets ships actual angular impulse."), Category = "MachRace|System")
	FVector GetFuselageAngularImpulse();
	
};

