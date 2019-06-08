// Copyright 2015 - Max Enko

#include "RaceLevelBase.h"
#include "MachRace.h"

// Sets default values
ARaceLevelBase::ARaceLevelBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceLevelBase::BeginPlay(){
	Super::BeginPlay();
	
}

// Called every frame
void ARaceLevelBase::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );
	
}

