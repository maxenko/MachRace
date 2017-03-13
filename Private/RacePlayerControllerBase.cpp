// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RacePlayerControllerBase.h"

ARacePlayerControllerBase::ARacePlayerControllerBase() {
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARacePlayerControllerBase::BeginPlay() {
	Super::BeginPlay();

}


// Called every frame
void ARacePlayerControllerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	auto state = GetState();
	if (state) {
		auto shipOk = false;
		auto ship = state->GetRaceShip(shipOk);

		// if no lateral input is being received, decay lateral movement to 0
		if (shipOk) {
										// if neither are down						// if both are down
			ship->EnableLateralDecay = (!InputRightEnabled && !InputLeftEnabled) || (InputRightEnabled && InputLeftEnabled);

			if (ship->EnableLateralDecay) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("delta: %f"), DeltaSeconds));
			} else {
				ship->Yaw(!InputRightEnabled);				
			}
		}
	}
}

ARaceGameStateBase* ARacePlayerControllerBase::GetState() {
	return UCustomExtensions::GetRaceState(GetWorld());
}

void ARacePlayerControllerBase::Bank(float alpha) {
	auto state = GetState();

	// meet require conditions to receive input
	if (!state && !state->IsInGame && !state->ControlsEnabled) {
		return;
	}

	auto shipOk = false;
	auto ship = state->GetRaceShip(shipOk);

	// if ship is not found, we can't make it bank
	if (!shipOk) {
		return;
	}
	auto speed = ship->GetTheoreticalSpeed();
	auto controls = state->GetControlSettings(speed);

	// cancel if already at max speed
	if (ship->GetTheoreticalBankingSpeed() >= controls.MaxBankingSpeed) {
		return;
	}
	auto negOrPos = alpha < 0 ? -1 : 1;
	auto multiplier = BankingCurve->GetFloatValue(FMath::Abs(alpha)) * negOrPos;
	auto finalImpulse = FVector(0, controls.BankingSpeedImpulse * multiplier, 0);
	ship->Bank(finalImpulse);

	ship->Spin(FVector(controls.BankingRotationImpulse*-negOrPos,0,0), 0/*controls.BankingRotationImpulseMax*/);
}

ARaceShipBase* ARacePlayerControllerBase::GetShip(bool &success) {
	auto state = GetState();
	if (state) {
		return state->GetRaceShip(success);
	}

	success = false;
	return NULL;
}