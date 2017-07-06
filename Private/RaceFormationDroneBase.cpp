// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceFormationDroneBase.h"
#include "X.h"

// Sets default values
ARaceFormationDroneBase::ARaceFormationDroneBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceFormationDroneBase::BeginPlay(){
	Super::BeginPlay();

	// set up wobble
	auto wobbleFrequency = FMath::FRandRange(WobbleRandomRange.X, WobbleRandomRange.Y);
	GetWorldTimerManager().SetTimer(wobbleTimer, this, &ARaceFormationDroneBase::generateRandomOffset, wobbleFrequency, true, 2);
}

void ARaceFormationDroneBase::moveTo(FVector to, float delta, FVector speed) {

	auto currentLoc = GetActorLocation();

	auto x = FMath::FInterpTo(currentLoc.X, to.X, delta, speed.X);
	auto y = FMath::FInterpTo(currentLoc.Y, to.Y, delta, speed.Y);
	auto z = FMath::FInterpTo(currentLoc.Z, to.Z, delta, speed.Z);

	SetActorLocation(FVector(x, y, z));
}

// Called every frame
void ARaceFormationDroneBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (abandoning) {
		FVector to = FVector::ZeroVector;
		auto currentLoc = GetActorLocation();

		if (!Position) {
			to = currentLoc + AbandonOffset;
		} else {
			to = Position->GetComponentLocation() + AbandonOffset;
		}

		moveTo(to, DeltaTime, AbandonSpeed);

	} else {

		if (Position) {
			auto offset = Wobble ? wobbleOffset : FVector::ZeroVector;
			moveTo(Position->GetComponentLocation() + offset, DeltaTime, FollowSpeed);
		}
	}
}

void  ARaceFormationDroneBase::AssignPosition(USceneComponent* position) {
	Position = position;
}

void ARaceFormationDroneBase::AbandonFormation() {
	abandoning = true;
}

void ARaceFormationDroneBase::generateRandomOffset() {
	if (Wobble) {
		wobbleOffset = UX::RandVecInRange(WobbleOffsetMin, WobbleOffsetMax);
	}
}