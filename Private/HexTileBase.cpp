// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "HexTileBase.h"



void AHexTileBase::projectGrid() {

	float cellHeight = CellSize * 2;
	float cellWidth = FMath::Sqrt(3) / 2 * cellHeight;

	int32 rows = FMath::FloorToInt(VerticalRange / cellHeight);
	int32 cols = FMath::FloorToInt(HorisontalRange / cellWidth);

	auto generateCellT = [&](int32 r, int32 c) {

		float x = r * cellHeight;
		float y = c * cellWidth;
		
		return FTransform(
			FVector(x, y, 0)
		);
	};

	for (int r = 0; r < rows; ++r) {

		for (int c = 0; c < cols; ++c) {

			// generate a cell transform
			this->Grid.Add(generateCellT(r,c));
		}
	}
}

void AHexTileBase::GenerateCoordinateGrid() {
	this->projectGrid();

}

