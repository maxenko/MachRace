// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DesertTileBase.h"
#include "RaceShipBase.h"


// Sets default values
ADesertTileBase::ADesertTileBase() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}


ARaceContentInstancer* ADesertTileBase::GetInstancer(bool& success) {
	auto stateOK = false;
	auto state = GetState(stateOK);

	if (!stateOK) {
		success = false;
		return NULL;
	}

	auto instancer = state->GetInstancer(success);
	return  instancer;
}

int32 ADesertTileBase::GetObstacleCount() {

	bool stateOk = false;
	auto state = GetState(stateOk);
	if (stateOk) {

		// check if override is on
		if (state->DisableObstacles) {
			return 0;
		}

		bool shipOk = false;
		ARaceShipBase* ship = state->GetRaceShip(shipOk);
		if (shipOk) {

			float speed = ship->GetTheoreticalSpeed();

			if (speed <= 1000) {
				return 0;
			} else if (speed < 1200) {
				return 10;
			} else if (speed < 1500) {
				return 12;
			} else if (speed < 1700) {
				return 13;
			} else if (speed < 1800) {
				return 14;
			} else if (speed < 1900) {
				return 15;
			} else if (speed < 2200) {
				return 10;
			} else if (speed > 2600) {
				return 0;
			}
		}
	}

	return 0;
}