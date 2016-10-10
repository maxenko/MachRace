// Copyright 2015 - Max Enko

#pragma once

#include "ShipBase.h"
#include "RaceShipBase.generated.h"




UCLASS()
class MACHRACE_API ARaceShipBase : public AShipBase {
	GENERATED_BODY()

private:
	UPrimitiveComponent* getRootAsPrimitive(bool& success);
	void decayLateralMovement(float delta);
	void decayRotationToZero(float delta);
	void recordFloorTraceToHistory();

public:

	ARaceShipBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Accelerate", Keywords = "Aceelerate the ship."), Category = "MachRace|System")
	void Accelerate(float forwardVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MinDistFromGround = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYaw = 4.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingYawSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	int32 FloorTraceHistorySize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	TArray<FVector> FloorTraceHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool EnableLateralDecay = false;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Ground Dist", Keywords = "Check and record gorund distance to history."), Category = "MachRace|System")
	void CheckGroundDist();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bank", Keywords = "Banks ship."), Category = "MachRace|System")
	void Bank(FVector impulse);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spin", Keywords = "Spin ship."), Category = "MachRace|System")
	FVector Spin(FVector impulse, float maxX = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Yaw", Keywords = "Yaw ship."), Category = "MachRace|System")
	float Yaw(bool direction);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Theoretical Speed", Keywords = "Gets theoretical ship speed."), Category = "MachRace|System")
	float GetTheoreticalSpeed();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Actual Speed", Keywords = "Gets actual ship speed."), Category = "MachRace|System")
	float GetSpeed();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Theoretical Lateral Speed", Keywords = "Gets theoretical lateral ship speed."), Category = "MachRace|System")
	float GetTheoreticalBankingSpeed();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Actual Lateral Speed", Keywords = "Gets actual lateral ship speed."), Category = "MachRace|System")
	float GetBankingSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Ships Angular Impulse.", Keywords = "Gets ships actual angular impulse."), Category = "MachRace|System")
	FVector GetFuselageAngularImpulse();
	
};

