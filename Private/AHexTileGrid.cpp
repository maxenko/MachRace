// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "AHexTileGrid.h"
#include "HexTilePiece.h"


// Sets default values
AHexTileGrid::AHexTileGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHexTileGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHexTileGrid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

TArray<FVector> AHexTileGrid::GenerateSpawnCoordinates(int w, int h, float cellSize) {
	return TArray<FVector>();
}

void AHexTileGrid::SpawnMainSection(UClass* typeOf, const FTransform t) {
	
	// only works with AActor
	if (!typeOf->IsChildOf(AActor::StaticClass())) {
		return; 
	}

	auto gridWidth		= cols * cellSizeW;
	auto gridHeight		= rows * cellSizeH;
	auto gridHalfWidth	= gridWidth / 2.0;
	auto gridHalfHeight = gridHeight / 2.0;

	auto rY1 = FVector2D(0.0, this->cols);
	auto rY2 = FVector2D(-gridHalfWidth, gridHalfWidth);

	auto rX1 = FVector2D(0.0, this->rows);
	auto rX2 = FVector2D(-gridHalfHeight, gridHalfHeight);

	for (int32 r = 0; r < this->rows; ++r) {

		auto xC = FMath::GetMappedRangeValueUnclamped(rX1, rX2, r); // x coordinate, based on grid size

		for (int32 c = 0; c < this->cols; ++c) {

			auto yC = FMath::GetMappedRangeValueUnclamped(rY1, rY2, c); // y coordinate, based on grid size

			auto childActor = NewObject<UChildActorComponent>(this);
			childActor->SetChildActorClass(typeOf);
			childActor->RegisterComponent();
			childActor->AttachToComponent(
				GetRootComponent(),
				FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
				NAME_None);
			childActor->SetRelativeTransform(t);
		}
	}
}

