// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "EnvironmentScanner.h"


// Sets default values for this component's properties
UEnvironmentScanner::UEnvironmentScanner(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnvironmentScanner::BeginPlay(){
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnvironmentScanner::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ){
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

TArray<FFlightNavigationRay> UEnvironmentScanner::Scan() {
	return TArray<FFlightNavigationRay>();
}