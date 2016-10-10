// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MechSnake.h"


// Sets default values
AMechSnake::AMechSnake()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMechSnake::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMechSnake::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

