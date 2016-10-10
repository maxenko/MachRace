// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MenuActor.h"


// Sets default values
AMenuActor::AMenuActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMenuActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMenuActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

