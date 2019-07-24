// Copyright 2015 - Max Enko

#include "RacePlayerControllerBase.h"
#include "MachRace.h"
#include "X.h"


ARacePlayerControllerBase::ARacePlayerControllerBase() {
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARacePlayerControllerBase::BeginPlay() {
	Super::BeginPlay();

	State = GetState();
	if (State) {
		auto shipOk = false;
		Ship = State->GetRaceShip(shipOk);

		tickLogicOkToRun = shipOk;
	}
}


// Called every frame
void ARacePlayerControllerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!tickLogicOkToRun) {
		return;
	}

	// if no lateral input is being received, decay lateral movement to 0
								// if neither are down						// if both are down
	Ship->EnableLateralDecay = (!InputRightEnabled && !InputLeftEnabled) || (InputRightEnabled && InputLeftEnabled);

	if (Ship->EnableLateralDecay) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("delta: %f"), DeltaSeconds));
	} else {
		Ship->Yaw(!InputRightEnabled);
	}
}

ARaceGameStateBase* ARacePlayerControllerBase::GetState() {
	return UX::GetRaceState(GetWorld());
}

void ARacePlayerControllerBase::Bank(float alpha) {

	auto speed = Ship->GetTheoreticalSpeed();
	auto bankingImpulse = Ship->ShipData->GetBankingSpeedImpulse(State->Stage);
	//auto controls = State->GetControlSettings(speed);

	// cancel if already at max speed
	if (Ship->GetTheoreticalBankingSpeed() >= bankingImpulse) {
		return;
	}

	auto finalImpulse = FVector(0, bankingImpulse * alpha, 0);
	Ship->Bank(finalImpulse);
}

ARaceShipBase* ARacePlayerControllerBase::GetShip(bool &success) {
	auto state = GetState();
	if (state) {
		return state->GetRaceShip(success);
	}

	success = false;
	return NULL;
}