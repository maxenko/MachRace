// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "BasicRaceActor.h"
#include "RaceActorBase.h"
#include "RaceShipBase.h"


// Sets default values
ABasicRaceActor::ABasicRaceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIgnoresOriginShifting = false;
}

// Called when the game starts or when spawned
void ABasicRaceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicRaceActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called every frame
ARaceGameStateBase* ABasicRaceActor::GetState(bool& success) {
	auto state = UX::GetRaceState(GetWorld());
	if (state) {
		success = true;
	} else {
		success = false;
	}

	return state;
}