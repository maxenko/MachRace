// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DrawDebugHelpers.h"
#include "HexTileBase.h"


void AHexTileBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (DrawDebug) {
		drawDebug();
	}
}

void AHexTileBase::projectGrid() {

	float cellHeight = CellSize * 2;
	float cellWidth = FMath::Sqrt(3) / 1.5 * cellHeight;

	int32 rows = FMath::FloorToInt(VerticalRange / cellHeight);
	int32 cols = FMath::FloorToInt(HorisontalRange / cellWidth);
	float yOffset = cols * cellWidth/2; // offset grid to make origin in the middle

	auto generateCellT = [&](int32 r, int32 c) {

		float x = r * cellHeight;
		float y = c * cellWidth + (r % 2 == 1 ? cellWidth/2 : 0); // offset each odd row

		return FTransform(
			FVector(x, y - yOffset, 0)
		);
	};

	for (int r = 0; r < rows; ++r) {

		for (int c = 0; c < cols; ++c) {

			// generate a cell transform
			this->Grid.Add(generateCellT(r,c));

		}
	}
}


bool AHexTileBase::isWithinThreshold(FVector v) {
	
	if (!Target) {
		return false;
	}

	auto cm = UGameplayStatics::GetPlayerCameraManager(this, 0);
	
	auto forward = cm->GetCameraRotation().Vector(); // look at normal
	FVector directional = Target->GetActorLocation() - v; // to target normal

	forward.Normalize();
	directional.Normalize();
	
	float dProductAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(forward, directional))); // angle to target

	return FMath::Abs(VisibleAngleThreshold) > FMath::Abs(180-dProductAngle);
}

void AHexTileBase::drawDebug() {
	for (FTransform t : Grid) {
		if (isWithinThreshold(t.GetLocation())) {
			DrawDebugPoint(GetWorld(), t.GetLocation(), 30, FColor::Red, false, .08, 0);
		}
	}
}

void AHexTileBase::GenerateCoordinateGrid() {
	this->projectGrid();
}

TArray<float> AHexTileBase::CalcDistancesToTarget() {

	if (!Target || Grid.Num() == 0) {
		return TArray<float>();
	}

	TArray<float> distances;

	FVector targetWorldPos = Target->GetActorLocation();

	for (FTransform t : Grid) {
		distances.Add(FVector::Dist(targetWorldPos, t.GetLocation()));
		//isWithinThreshold(t.GetLocation());
	}

	return distances;
}
