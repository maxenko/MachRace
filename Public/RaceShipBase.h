// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "ShipData.h"
#include "RaceShipBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldCharge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDeplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverchargeAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldActivity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoostIncreased, int32, count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoostUsed, int32, count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccelerate, float, SpeedIncrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecelerate, float, SpeedDecrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpeedChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMachSpeedChange, int32, MachSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBank, int32, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIgnition, bool, OnOff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccelerationFlash, float, Multiplier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecelerationFlash, float, Multiplier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfLevelBounds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitOfPlayableArea);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEntryOfPlayableArea);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExploded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldActivated);

UCLASS()
class MACHRACE_API ARaceShipBase : public ARaceActorBase {
	GENERATED_BODY()

private:
	UPrimitiveComponent* getRootAsPrimitive(bool& success);
	void decayLateralMovement(float delta);
	void decayRotationToZero(float delta);
	int32 previousMach = 0;
	int32 previousOverchargeCount = 0;
	void changeSpeed(float by);
	bool level1IsShipOutOfBounds(AActor* tile);

public:

	ARaceShipBase();


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Ship Speed", Keywords = "Sets speed ship to a specific (theoretical) value."), Category = "MachRace|Utility")
	void SetShipSpeed(float speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	UShipData* ShipData = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool EnableOvercharge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ShieldHitPoints = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ShieldMaxHitPoints = 500;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float OverchargeTotal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	int32 BoostCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FColor LaserColor;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Charge Shield", Keywords = "Recharge shield hitpoints."), Category = "MachRace|Gameplay")
	void ChargeShield(float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Deplete Shield", Keywords = "Deplete shield hitpoints."), Category = "MachRace|Gameplay")
	void DepleteShield(float amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Overcharge Count", Keywords = "Gets total count of usable overcharges."), Category = "MachRace|Gameplay")
	int32 GetOverchargeCount();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset Overcharge Count", Keywords = "Reset total count of usable overcharges."), Category = "MachRace|Gameplay")
	int32 ResetOverchargeCount();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Delta Damage.", Keywords = "Takes damage with delta multiplier applied."), Category = "MachRace|Gameplay")
	void TakeDeltaDamage(float delta, float dmgAmount);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnShieldCharge OnShieldCharge;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnShieldDeplete OnShieldDeplete;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnShieldDepleted OnShieldDepleted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnShieldActivity OnShieldActivity;
	 
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnOverchargeAdded OnOverchargeAdded;



	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Accelerate", Keywords = "Accelerate the ship."), Category = "MachRace|Gameplay")
	void Accelerate(float forwardVelocity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Decelerate", Keywords = "Decelerate the ship."), Category = "MachRace|Gameplay")
	void Decelerate(float forwardVelocity);

	float MinDistFromGroundCurrent = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MinDistFromGround = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float GroundFollowSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYaw = 4.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYawSpeed = 10;

	/// used to calculate forward velocity fluctuations on various hits, hits that don't slow 
	/// the ship down enough shouldn't explode it.
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	float PreviousXVelocity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxVelocityChangeThreshold = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ShipHoverRealignmentSpeed = .6;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsVelocityChangeFatal", Keywords = "Checks wether velocity change between now and last time should be treated as fatal collision."), Category = "MachRace|Gameplay")
	bool IsVelocityChangeFatal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool EnableLateralDecay = false;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnAccelerate OnAccelerate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnDecelerate OnDecelerate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnSpeedChange OnSpeedChange;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Level1Bound = 1000;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnOutOfLevelBounds OnOutOfLevelBounds;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnExploded OnExploded;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnExitOfPlayableArea OnExitOfPlayableArea;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnShieldActivated OnShieldActivated;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Trigger Exit Playable Area", Keywords = "Triggers OnExitOfPlayableArea."), Category = "MachRace|Gameplay")
	void TriggerExitPlayableArea() {
		OnExitOfPlayableArea.Broadcast();
	}

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnEntryOfPlayableArea OnEntryOfPlayableArea;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Trigger Enter Playable Area", Keywords = "Triggers OnExitOfPlayableArea."), Category = "MachRace|Gameplay")
	void TriggerEnterPlayableArea() {
		OnEntryOfPlayableArea.Broadcast();
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Broadcast exploded.", Keywords = "Notifies all subscribers, that ship has exploded."), Category = "MachRace|System")
	void BroadcastExploded();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Trigger Ignition", Keywords = "Starts ignition sequence, purely cosmetic."), Category = "MachRace|Presentation")
	void TriggerIgnition(bool onOff);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Ground Dist", Keywords = "Check and record gorund distance to history."), Category = "MachRace|System")
	float CheckGroundDist(FHitResult& hit);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bank", Keywords = "Banks ship."), Category = "MachRace|System")
	void Bank(FVector impulse);

	/*
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spin", Keywords = "Spin ship."), Category = "MachRace|System")
	FVector Spin(FVector impulse, float maxX = 0);*/

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

