// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "PulseLaserBase.h"


// Sets default values
APulseLaserBase::APulseLaserBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APulseLaserBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APulseLaserBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

