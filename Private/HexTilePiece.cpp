// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "HexTilePiece.h"

#include "CustomExtensions.h"

// Sets default values
AHexTilePiece::AHexTilePiece(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHexTilePiece::OnConstruction(const FTransform & Transform){
	this->SetActorScale3D(FVector(1,1,1)*InstigatorScale);
}

// Called when the game starts or when spawned
void AHexTilePiece::BeginPlay(){
	Super::BeginPlay();
}

// Called every frame
void AHexTilePiece::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );

	bool destroy = targetWorldLoc.X < (GetActorLocation().X - 3000);
	if (destroy) {
		unRegisterIndex(this->GridIndex);
		this->DoDeferredDestroy = true;
		//this->Destroy();
	}
}

void AHexTilePiece::PostInitProperties() {
	Super::PostInitProperties();

	// if there nothing in the map (no actors specified to spawn), we don't proceed
	if (WeightMap.Num() <= 0) {
		return;
	}

	// unwrap weight map
	for (FHexTilePieceElementMeta m : WeightMap) {
		for (int i = 0; i < m.Weight; ++i) {
			unwrappedWeightMap.Add(m);
		}
	}

	// only even rows, otherwise tiles wont match
	if (this->Rows % 2 == 1) {
		this->Rows++;
	}
}

void AHexTilePiece::PostInitializeComponents() {
	Super::PostInitializeComponents();
}


UClass* AHexTilePiece::GetRandomFromWeightMap() {
	auto wrappedIdx = UCustomExtensions::WrapIdxTo(unwrappedWeightMap.Num() - 1, FMath::Rand());
	return unwrappedWeightMap[wrappedIdx].Type;
}

AMachGameStateBase* const AHexTilePiece::GetMachGameState() {
	return GetWorld()->GetGameState<AMachGameStateBase>();;
}


bool AHexTilePiece::IsWithinThreshold(FVector to) {
	auto thisLoc = GetActorLocation();
	auto dist = (to - thisLoc).Size();

	return dist < PropagateThreshhold;
}


bool AHexTilePiece::isInTheGrid(FVector v) {

	auto state = GetMachGameState();

	if (!state) {
		return true; // we don't want to spawn if there is no way to handle duplicates
	}

	return state->Level2Index.Contains(v);;
}


bool AHexTilePiece::registerIndex(FVector index) {
	auto state = GetMachGameState();
	if (state) {
		state->Level2Index.Add(index);
		return true;
	} else {
		return false;
	}
}

bool AHexTilePiece::unRegisterIndex(FVector index) {
	auto state = GetMachGameState();
	if (state) {
		state->Level2Index.Remove(index);
		return true;
	} else {
		return false;
	}
}

FVector  AHexTilePiece::GetForwardTileSpawnPos() {

	// forward is -X

	auto currentWorldPos = this->GetActorLocation();
	auto rScale = this->GetActorScale();
	
	float height = CellSize * 2;
	auto offset = (height * (Columns - 1) + (CellSize/FMath::Sqrt(3)/2))*rScale.X;
	
	return FVector((currentWorldPos.X - offset*InstigatorScale), currentWorldPos.Y, currentWorldPos.Z);
}

FVector AHexTilePiece::GetLeftTileSpawnPos() {

	// left is +Y

	auto currentWorldPos = this->GetActorLocation();
	auto rScale = this->GetActorScale();

	float height = CellSize * 2;
	float width = FMath::Sqrt(3) / 2 * height;
	float offset = ((width / FMath::Sqrt(3)) * (Rows/2))*rScale.Y;
	
	return FVector(currentWorldPos.X, (currentWorldPos.Y + offset*InstigatorScale), currentWorldPos.Z);
}

FVector AHexTilePiece::GetRightTileSpawnPos() {

	// right is -Y

	auto currentWorldPos = this->GetActorLocation();
	auto rScale = this->GetActorScale();

	float height = CellSize * 2;
	float width = FMath::Sqrt(3) / 2 * height;
	float offset = ((width / FMath::Sqrt(3)) * (Rows/2))*rScale.Y;
	
	return FVector(currentWorldPos.X, (currentWorldPos.Y - offset*InstigatorScale), currentWorldPos.Z);
}

void AHexTilePiece::SpawnLeft(UClass* tile) {

	// Left direction is +Y

	FVector wouldBeLeftIndex = FVector(0, 1, 0) + GridIndex;

	if (DidSpawnLeft || isInTheGrid(wouldBeLeftIndex)) {
		return;
	}

	auto world = this->GetWorld();
	auto spawnInfo = FActorSpawnParameters();
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	const FTransform t = FTransform(FRotator(0,0,0), GetLeftTileSpawnPos(), GetActorScale3D());

	if (world) {

		// do some special stuff if its a AHexTilePiece
		if ( tile->IsChildOf(AHexTilePiece::StaticClass() ) ) {
			
			auto spawned = world->SpawnActor<AHexTilePiece>(tile, t, spawnInfo);

			if (spawned) {

				spawned->DidSpawnRight = true; // prevent spawn of right tile (since this is the right tile)
				spawned->GridIndex = wouldBeLeftIndex;
				this->registerIndex(spawned->GridIndex);
				spawned->CanPropagate = true; // enable player detection and spawn loop
				DidSpawnLeft = true;
			}

		// Otherwise spawn regular actor.
		} else {
			auto spawned = world->SpawnActor<AActor>(tile, t, spawnInfo);
			if (spawned) {
				DidSpawnLeft = true;
			}
		}
	}
}

void AHexTilePiece::SpawnRight(UClass* tile) {

	// Right direction is -Y

	FVector wouldBeRightIndex = FVector(0, -1, 0) + GridIndex;

	if (DidSpawnRight || isInTheGrid(wouldBeRightIndex)) {
		return;
	}

	auto world = this->GetWorld();
	auto spawnInfo = FActorSpawnParameters();
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FTransform t = FTransform(FRotator(0, 0, 0), GetRightTileSpawnPos(), GetActorScale3D());

	if (world) {

		// do some special stuff if its a AHexTilePiece
		if (tile->IsChildOf(AHexTilePiece::StaticClass())) {

			auto spawned = world->SpawnActor<AHexTilePiece>(tile, t, spawnInfo);

			if (spawned) {

				spawned->DidSpawnLeft = true; // prevent spawn of right tile (since this is the right tile)
				spawned->GridIndex = wouldBeRightIndex;
				this->registerIndex(spawned->GridIndex);
				spawned->CanPropagate = true; // enable player detection and spawn loop
				DidSpawnRight = true;
			}

			// Otherwise spawn regular actor.
		} else {
			auto spawned = world->SpawnActor<AActor>(tile, t, spawnInfo);
			if (spawned) {
				DidSpawnRight = true;
			}
		}
	}
}

void AHexTilePiece::SpawnForward(UClass* tile) {

	// Forward direction is -X

	FVector wouldBeForwardIndex = FVector(-1, 0, 0) + GridIndex;

	if (DidSpawnForward || isInTheGrid(wouldBeForwardIndex)) {
		return;
	}

	auto world = this->GetWorld();
	auto spawnInfo = FActorSpawnParameters();
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FTransform t = FTransform(FRotator(0, 0, 0), GetForwardTileSpawnPos(), GetActorScale3D());

	if (world) {

		// do some special stuff if its a AHexTilePiece
		if (tile->IsChildOf(AHexTilePiece::StaticClass())) {
			auto spawned = world->SpawnActor<AHexTilePiece>(tile, t, spawnInfo);

			if (spawned) {
				spawned->CanPropagate = true; // enable player detection and spawn loop
				spawned->GridIndex = wouldBeForwardIndex;
				this->registerIndex(spawned->GridIndex);
				DidSpawnForward = true;
			}

			// Otherwise spawn regular actor.
		} else {
			auto spawned = world->SpawnActor<AActor>(tile, t, spawnInfo);
			if (spawned) {
				DidSpawnForward = true;
			}
		}
	}
}

