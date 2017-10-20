// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DrawDebugHelpers.h"
#include "RaceShipBase.h"
#include "HexTileBase.h"
#include "Kismet/KismetMathLibrary.h"


void AHexTileBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// pull some stuff we're going to need every frame
	auto cm = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (cm) {
		cameraLoc = cm->GetCameraLocation();
		cameraDir = cm->GetCameraRotation().Vector();
		cameraDir.Normalize();
		ScanVisibleGrid(EnableSpawns);
	}
}

void AHexTileBase::BeginPlay() {
	Super::BeginPlay();

	auto state = GetState();
	if (state) {
		state->CountHexTile();
	}


	weights = GenerateDistributionMap();
	auto d = normilizeDistribution(weights);
	int32 mapSize = 1000;
	chances.Reserve(mapSize);

	// add specific enum selection N amount of times depending on the weight derived out of mapSize
	auto distribute = [&](EHexTileChance t, float weight) {

		int32 chunkSize = UKismetMathLibrary::FTrunc(weight * (float)mapSize);

		for (int i = 0; i < chunkSize; ++i) {
			chances.Add(t);
		}
	};

	// distribute chances (fills the array for random pick)
	distribute(EHexTileChance::Accelerator, d.Accelerators);
	distribute(EHexTileChance::Collectable, d.Collectables);
	distribute(EHexTileChance::Decelerator, d.Decelerators);
	distribute(EHexTileChance::ICBM, d.ICBM);
	distribute(EHexTileChance::Standard, d.Standard);
	distribute(EHexTileChance::Column, d.Column);

}


void AHexTileBase::projectGrid() {

	float cellHeight = CellSize * 2;
	float cellWidth = FMath::Sqrt(3) / 1.5 * cellHeight;

	int32 rows = FMath::FloorToInt(VerticalRange / cellHeight);
	int32 cols = FMath::FloorToInt(HorisontalRange / cellWidth);
	float yOffset = cols * cellWidth/2; // offset grid to make origin in the middle
	float xOffset = rows * cellHeight;

	auto generateCellT = [&](int32 r, int32 c) {

		float x = r * cellHeight;
		float y = c * cellWidth + (r % 2 == 1 ? cellWidth/2 : 0); // offset each odd row

		return FTransform(
			FVector(x - xOffset, y - yOffset, 0)
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

	auto dist = FVector::Dist(v, cameraLoc );
	
	// if we are too far from the point, no needs to calculate anything further
	if (dist > DistanceThreshold) {
		return false;
	}

	FVector directional = cameraLoc - v; // to target normal
	directional.Normalize();
	
	// angle between vectors (we're concerned whether or not vector falls inside the visible cone & dist
	float dProductAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(cameraDir, directional))); // angle to target

	return FMath::Abs(VisibleAngleThreshold) > FMath::Abs(180 - dProductAngle);
}

void AHexTileBase::GenerateCoordinateGrid() {
	this->projectGrid();
}

void AHexTileBase::ScanVisibleGrid(bool triggerSpawns) {
	for (FTransform t : Grid) {
		auto worldLoc = t.GetLocation() + GetActorLocation();
		if ( isWithinThreshold(worldLoc) ) {

			// if index isn't in SpawnIndex and we can trigger spawn, notify subscriber(s)
			if (triggerSpawns && !IndexTaken(t)) {
				OnAvailableIndex.Broadcast(t, t * GetActorTransform());
			}

			if (DrawDebug) {
				FColor debugDrawColor = FColor::Green;
				if (IndexTaken(t)) {
					debugDrawColor = FColor::Red;
				}
				DrawDebugPoint(GetWorld(), worldLoc, 10, debugDrawColor, false, .08, 0);
			}
		} else {
			if (DrawDebug) {
				DrawDebugPoint(GetWorld(), worldLoc, 3, FColor::Black, false, .08, 0);
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

FHexTileDistribuition AHexTileBase::GenerateDistributionMap() {

	FHexTileDistribuition d;

	auto state = GetState();
	if (!state) {
		return d;
	}

	bool shipOk;
	auto ship = state->GetRaceShip(shipOk);

	if (!shipOk) {
		return d;
	}

	// always update the standard weight based on all other weights 
	// (this is the amount of standard hex pieces that is left over after all other pieces)
	auto adjust = [](FHexTileDistribuition d) {
		d.Standard = 1.0f - (d.Decelerators + d.Accelerators + d.Collectables + d.ICBM + d.Column);
		return d;
	};

	auto speed = ship->GetTheoreticalSpeed();

	if(state->Stage == GameStage::InfiniteHex){

		if (speed < state->Level2ObstacleTriggerspeed) {

			d.Decelerators = .02;
			d.Accelerators = .03;
			d.Collectables = .004;
			d.ICBM		   = .000;
			d.Column	   = .000;
		
			return adjust(d);
		}
	
		if (speed >= state->Level2ObstacleTriggerspeed) { // check RaceGameStateBase.cpp where this is also used in tandem

			d.Decelerators = .04;
			d.Accelerators = .06;
			d.Collectables = .005;
			d.ICBM = .003;
			d.Column = .005;
			return adjust(d);
		}

	} else if (state->Stage == GameStage::InfiniteHexBoss) {

		d.Decelerators = .00;
		d.Accelerators = .00;
		d.Collectables = .005;
		d.ICBM = 0.0;
		d.Column = 0.0;
		return adjust(d);

	}

	return d;
}

FHexTileDistribuition AHexTileBase::normilizeDistribution(FHexTileDistribuition d) {

	float sum = d.Accelerators + d.Collectables + d.Column + d.Decelerators + d.ICBM + d.Standard;

	if (sum == 1.0) {

		return d;
	
	} else {

		auto a = FVector2D(0, sum);
		auto b = FVector2D(0, 1);

		d.Accelerators		= FMath::GetMappedRangeValueClamped(a, b, d.Accelerators);
		d.Collectables		= FMath::GetMappedRangeValueClamped(a, b, d.Collectables);
		d.Column			= FMath::GetMappedRangeValueClamped(a, b, d.Column);
		d.Decelerators		= FMath::GetMappedRangeValueClamped(a, b, d.Decelerators);
		d.ICBM				= FMath::GetMappedRangeValueClamped(a, b, d.ICBM);
		d.Standard			= FMath::GetMappedRangeValueClamped(a, b, d.Standard);

		return d;
	}
}

void AHexTileBase::HexTileChanceSpawn(EHexTileChance& Branches) {

	if (chances.Num() <= 0) {
		return;
	}

	// random index!
	int32 pick = FMath::RandRange(0, chances.Num() - 1);

	Branches = chances[pick];
}

FVector AHexTileBase::GetNextTileSpawnPos() {

	auto state = GetState();
	if (!state) {
		return FVector::ZeroVector;
	}

	bool shipOk;
	auto ship = state->GetRaceShip(shipOk);

	if (!shipOk) {
		return FVector::ZeroVector;
	}

	float cellHeight = CellSize * 2;
	int32 rows = FMath::FloorToInt(VerticalRange / cellHeight);
	float xOffset = (rows) * -cellHeight;

	return FVector( xOffset + GetActorLocation().X, ship->GetActorLocation().Y, GetActorLocation().Z );
}