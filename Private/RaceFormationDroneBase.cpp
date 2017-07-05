// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceFormationDroneBase.h"

// Sets default values
ARaceFormationDroneBase::ARaceFormationDroneBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceFormationDroneBase::BeginPlay(){
	Super::BeginPlay();

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

		auto x = FMath::FInterpTo(currentLoc.X, to.X, DeltaTime, AbandonSpeed.X);
		auto y = FMath::FInterpTo(currentLoc.Y, to.Y, DeltaTime, AbandonSpeed.Y);
		auto z = FMath::FInterpTo(currentLoc.Z, to.Z, DeltaTime, AbandonSpeed.Z);

		SetActorLocation(FVector(x, y, z));
	}
}

void  ARaceFormationDroneBase::AssignPosition(USceneComponent* position) {
	Position = position;
}

void ARaceFormationDroneBase::AbandonFormation() {
	abandoning = true;
}