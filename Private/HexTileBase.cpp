// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DrawDebugHelpers.h"
#include "HexTileBase.h"


void AHexTileBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	ScanVisibleGrid(EnableSpawns);
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

	auto cm = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!cm) {
		return false;
	}

	auto dist = FVector::Dist(v, cm->GetCameraLocation());
	
	// if we are too far from the point, no needs to calculate anything further
	if (dist > DistanceThreshold) {
		return false;
	}
	
	auto forward = cm->GetCameraRotation().Vector(); // look at normal
	FVector directional = UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation() - v; // to target normal

	forward.Normalize();
	directional.Normalize();
	
	// angle between vectors (we're concerned whether or not vector falls inside the visible cone & dist
	float dProductAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(forward, directional))); // angle to target

	return FMath::Abs(VisibleAngleThreshold) > FMath::Abs(180-dProductAngle); 
}

void AHexTileBase::GenerateCoordinateGrid() {
	this->projectGrid();
}

void AHexTileBase::ScanVisibleGrid(bool triggerSpawns) {
	for (FTransform t : Grid) {
		if (isWithinThreshold(t.GetLocation() + GetActorLocation())) {

			

			// if index isn't in SpawnIndex and we can trigger spawn, notify subscriber(s)
			if (triggerSpawns && !IndexTaken(t)) {
				OnAvailableIndex.Broadcast(t, t * GetActorTransform());
			}

			if (DrawDebug) {
				FColor debugDrawColor = FColor::Green;
				if (IndexTaken(t)) {
					debugDrawColor = FColor::Red;
				}
				DrawDebugPoint(GetWorld(), t.GetLocation(), 30, debugDrawColor, false, .08, 0);
			}
		}
	}
}

void AHexTileBase::TakeIndex(FTransform localT) {
	SpawnIndex.Add(localT);
}

bool AHexTileBase::IndexTaken(FTransform localT) {

	for (auto t : SpawnIndex) {
		if (
				t.GetLocation() == localT.GetLocation()
			&&	t.GetRotation() == localT.GetRotation()
			&&	t.GetScale3D()	== localT.GetScale3D()

			) {
			return true;
		}
	}

	return false;
}

