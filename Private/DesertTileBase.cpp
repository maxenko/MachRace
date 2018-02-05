// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceShipBase.h"
#include "DesertTileBase.h"


// Sets default values
ADesertTileBase::ADesertTileBase() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}


void ADesertTileBase::BeginPlay() {
	Super::BeginPlay();

	GetState()->AddIgnoredByLaserTrace(this);
}


ARaceContentInstancer* ADesertTileBase::GetInstancer(bool& success) {
	auto state = GetState();

	if (state) {
		success = false;
		return NULL;
	}

	auto instancer = state->GetInstancer(success);
	return  instancer;
}

int32 ADesertTileBase::GetObstacleCount() {

	auto state = GetState();
	if (state) {

		// check if override is on, this is likely a boss level, we run empty tiles
		if (state->DisableObstacles) {
			return 0;
		}

		bool shipOk = false;
		ARaceShipBase* ship = state->GetRaceShip(shipOk);
		if (shipOk) {

			float speed = ship->GetTheoreticalSpeed();

			// mines tile
			if (TileType == DesertTileType::Mines) {

				if (speed < 1200) {
					return 4;
				}
				else if (speed < 1500) {
					return 3;
				}
				else if (speed < 2000) {
					return 2;
				}
				else {
					return 0;
				}

			// regular tile
			} else {

				if (speed <= 1000) {
					return 0;
				}
				else if (speed < 1200) {
					return 10;
				}
				else if (speed < 1500) {
					return 12;
				}
				else if (speed < 1700) {
					return 13;
				}
				else if (speed < 1800) {
					return 14;
				}
				else if (speed < 1900) {
					return 15;
				}
				else if (speed < 2200) {
					return 10;
				}
				else if (speed < 2600) {
					return 7;
				}
				else if (speed > 2600) { // boss time!
					return 0;
				}
			}

		}
	}

	// catch all
	return 0;
}