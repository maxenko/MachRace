// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "CityAreaBase.h"
#include "RaceShipBase.h"


// Sets default values
ACityAreaBase::ACityAreaBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACityAreaBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACityAreaBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACityAreaBase::GenerateGrid() {

	// get this area tile location
	auto tileLoc = GetActorLocation();
	IntersectionsLocations.Empty();


	for (int32 ix = 0; ix < HalfGridX; ++ix) {
		for (int32 iy = 0; iy < HalfGridY; ++iy) {

			auto SpacingY = (ix % 2 == 0 ? 0 : Spacing);

			IntersectionsLocations.Add(
				FVector(
					tileLoc.X + (ix * Spacing)/1.95 - (HalfGridX/2*Spacing ),
					tileLoc.Y + (iy * Spacing) - (HalfGridY/2*Spacing + SpacingY/2),
					tileLoc.Z));
		}
	}
}