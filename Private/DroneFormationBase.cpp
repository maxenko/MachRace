// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DroneFormationBase.h"


// Sets default values
ADroneFormationBase::ADroneFormationBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADroneFormationBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADroneFormationBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

