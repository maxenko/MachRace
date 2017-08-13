// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "NumericLimits.h"
#include "X.h"
#include "DroneFormationBase.h"



// Sets default values
ADroneFormationBase::ADroneFormationBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADroneFormationBase::BeginPlay(){
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(spawnTimer, this, &ADroneFormationBase::broadcastDroneSpawn, DroneSpawnFrequence, true, 2.0); // issue spawns every DroneSpawnFrequence seconds
}

// Called every frame
void ADroneFormationBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (Columns == 0 && Rows == 0) {
		return;
	}

	detectAndProcessChanges();

	if (DrawDebug) {
		drawDebug();
	}

	// if spawns are enabled, fill the empty slots in the formation
	if (EnableSpawns) {
		for (auto link : Links) {

			if (link->Drone == NULL) {
				// add to spawn queue
				toBeSpawned.AddUnique(link);
			}
		}
	}

	// clean up null refs
	cleanDestroyedDrones();
}

void ADroneFormationBase::drawDebug() {

	// sanity check
	auto w = GetWorld();
	if (!w) {
		return;
	}

	for ( USceneComponent* p : Positions ){
		auto loc = p->GetComponentLocation();
		DrawDebugPoint(w, loc, 20, FColor::Red, false, .08);
	}

	DrawDebugBox(w, GetActorLocation(), Bounds, FColor::Red, false, .08,0,3.0);
}

void ADroneFormationBase::detectAndProcessChanges() {

	// if grid size changed, rebuild grid
	if (Columns != previousColumns || Rows != previousRows) {
		realignGrid();
		previousColumns = Columns;
		previousRows = Rows;
		OnGridUpdate.Broadcast();
	}
}

void ADroneFormationBase::realignGrid() {

	// allocate new positions array
	auto previous = Positions;

	auto grid = getFormationGrid();

	// sanity check
	if (grid.Num() != Columns*Rows) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("DroneFormation system error, grid sizing mismatch. %i != %i"), Columns*Rows, grid.Num())
		);
		return;
	}

	// clear old components
	auto components = GetComponentsByTag(USceneComponent::StaticClass(), gridMarkerTagName);
	for (auto r : components) {
		RemoveOwnedComponent(r);
	}

	Positions.Empty();

	// adds new positioning components using grid index
	// components are used to align drones to the grid
	for (auto& idx : grid) {
		auto c = NewObject<USceneComponent>(this);
		c->RegisterComponent();
		c->SetRelativeLocation(idx.Vector);
		c->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		c->ComponentTags.Add(gridMarkerTagName);
		Positions.Add(c);
		idx.Marker = c;
	}

	Index = grid;

	// relink drones to new positions
	relinkDrones();
}

/** Generates new rectangular grid of vectors and col/row positions */
TArray<FDroneFormationSquareIndex> ADroneFormationBase::getFormationGrid(){

	TArray<FDroneFormationSquareIndex> newIndex;
	newIndex.Reserve(Columns*Rows);

	auto spacingX = (Bounds.X*2)/(Rows-1);		// -1 offsets the fill so coordinates fill entire bounds
	auto spacingY = (Bounds.Y*2)/(Columns-1);	// with zero as start pos and bound*2 as end pos

	for(int32 y = 0; y < Columns; ++y){
		for(int32 x = 0; x < Rows; ++x){

			auto cy = (y*spacingY) - Bounds.Y;
			auto cx = (x*spacingX) - Bounds.X;

			FVector pos = FVector(cx, cy, 0);
			newIndex.Add(FDroneFormationSquareIndex(pos, y, x));
		}
	}

	return newIndex;
}

void ADroneFormationBase::relinkDrones() {

	TArray<ARaceFormationDroneBase*> newDrones;
	auto newDroneCount = Columns * Rows;
	newDrones.Reserve(newDroneCount);

	// rebuild links and position them in relative grid
	Links.Empty();
	for (auto i = 0; i < newDroneCount; ++i) {
		UDroneToFormationLink* link = NewObject<UDroneToFormationLink>(this);
		Links.Add(link);
		link->Position = Positions[i]; // positions count would always match here, as this method is called immediate after new positions are generated
	}

	// link existing drones based on their proximity to new positions
	for (auto i = 0; i < Positions.Num() && i < Drones.Num(); ++i) {

		auto loc = Positions[i]->GetComponentLocation();
		auto dist = TNumericLimits<float>::Max();
		ARaceFormationDroneBase* reassignedDrone = NULL;

		// loop through drones, assign each to the closest position
		for (auto n = 0; n < Drones.Num(); ++n) {
			auto droneLoc = Drones[n]->GetActorLocation();
			auto thisDist = FVector::Dist(loc, droneLoc);

			// is this one closer than the last one?
			if (thisDist < dist) {
				dist = thisDist;
				reassignedDrone = Drones[n];
			}
		}

		// reassign existing drone to new formation layout
		if (reassignedDrone) {
			Links[i]->Drone = reassignedDrone;
			Drones.Remove(reassignedDrone);
			newDrones.Add(reassignedDrone);
			OnReassignDrone.Broadcast(reassignedDrone, Links[i]->Position); // this will be used to reassign the drones in the blueprint that inherits from this class

			// link reassigned drone to the new index position
			for (auto& idx : Index) {
				if (idx.Marker == Links[i]->Position) {
					idx.Drone = reassignedDrone;
				}
			}
		}
	}

	// release any leftover drones from the existing set
	for (auto d : Drones) {
		OnReleaseDrone.Broadcast(d);
	}

	Drones = newDrones;
}

void ADroneFormationBase::LinkDrone(ARaceFormationDroneBase* drone, UDroneToFormationLink* link) {
	
	link->Drone = drone;
	if (!Drones.Contains(drone)) {
		Drones.Add(drone);
		Count++;

			// update index with drone info
		for (auto& i : Index) {
			if (i.Marker == link->Position) {
				i.Drone = link->Drone;

				// are column counts empty? this is usually the case on first drone link
				if (ColumnCounts.Num() <= 0) {
					ColumnCounts.AddZeroed(Columns);
				}

				// count drone by its column
				ColumnCounts[i.Column]++;
			}
		}
	}
}

ARaceFormationDroneBase* ADroneFormationBase::GetClosestDroneInAttackPosition( bool& success) {

	// sanity check
	if (!Target) {
		return NULL;
	}

	ARaceFormationDroneBase* drone = NULL;
	success = false;

	float previousShortestDist = TNumericLimits<float>::Max();

	for (auto d : Drones) {

		float yDist = UX::GetYDistBetweenActors(Target, d);

		// is yDist within threshold?
		if (yDist < AttackTriggerMinYDist) {
			float dist = Target->GetDistanceTo(d);
			if (dist < previousShortestDist) {
				previousShortestDist = dist;
				drone = d;
				success = true;
			}
		}
	}

	return drone;
}

ARaceFormationDroneBase* ADroneFormationBase::PickRandomDroneToDesignate(bool& success) {

	// pick random column that has drones
	auto hasColumnWithDrones = ColumnCounts.ContainsByPredicate([](int32 cnt) {return cnt > 0; });

	if (!hasColumnWithDrones) {
		success = false;
		return NULL;
	}

	TArray<int32> nonEmptyColIndexes;

	for (int32 i = 0; i < ColumnCounts.Num(); ++i) {
		if (ColumnCounts[i] > 0) {
			nonEmptyColIndexes.Add(i);
		}
	}

	int32 randomNonEmptyCol = FMath::RandRange(0, nonEmptyColIndexes.Num() - 1);

	ARaceFormationDroneBase* pickedDrone = NULL;

	for (auto idx : Index) {

		if (idx.Column != randomNonEmptyCol) {
			continue;
		}

		if (IsValid(idx.Drone)) {

			if (pickedDrone == NULL) {
				pickedDrone = idx.Drone;
			}
			else {
				if (pickedDrone->GetActorLocation().X < idx.Drone->GetActorLocation().X) {
					pickedDrone = idx.Drone;
				}
			}
		}
	}

	success = true;
	return pickedDrone;
}


bool ADroneFormationBase::isThereADesignatedDrone() {
	for (auto d : Drones) {
		if (!d) {
			continue;
		}

		if (!d->IsValidLowLevel()) {
			continue;
		}

		if (d->DesignatedDrone) {
			return true;
		}
	}

	return false;
}

bool ADroneFormationBase::AssignClosestDroneIfNoneAreDesignated() {
	if (!isThereADesignatedDrone()) {
		bool success = false;
		auto closest = GetClosestDroneInAttackPosition(success);
		if (success) {
			closest->DesignatedDrone = true;
			closest->Target = Target;
			return true;
		}
	}

	return false;
}

void ADroneFormationBase::cleanDestroyedDrones() {

	for (int32 n = Drones.Num(); n-- > 0;) {

		auto d = Drones[n];

		if (d->IsPendingKill()) {
  			Drones.RemoveAt(n);
			Count--;

			if (Count <= 0) {
				OnAllDronesDestroyed.Broadcast();
			}

			if (DrawDebug) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Formation Count: %i"), Count));
			}

			// update index with drone info
			for (auto i : Index) {
				if (i.Drone == d) {
					i.Drone = NULL;
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Set destroyed drone index to NULL!"));

					OnDroneDestroyed.Broadcast(i.Marker);
				}
			}

			// recalculate column sizes
			TArray<int32> sizes;
			sizes.AddZeroed(Columns);

			// recalculate column sizes
			// this is used for gameplay decisions
			for (auto& i : Index) {
				if (i.Drone) {
					if(!i.Drone->IsPendingKill()){
						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Drone column index: %i, Row: %i"), i.Column, i.Row));
						sizes[i.Column]++;
					}
				}
			}

			ColumnCounts = sizes;

			if (DrawDebug) {
				for (int32 i = 0; i < ColumnCounts.Num(); ++i) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
						FString::Printf(TEXT("Col: %i, size: %i"), i, ColumnCounts[i]));
				}
			}
		}
	}
}

int32 ADroneFormationBase::findLargestColumnSize() {

	// find largest column count
	int32 largest = 0;
	for (int32 colSize : ColumnCounts) {
		if (colSize > largest) {
			largest = colSize;
		}
	}

	if (DrawDebug) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Largest col: %i"), largest));
	}

	return largest;
}

float ADroneFormationBase::FindLogicalFormationOffset() {

	// sanity check
	if (Index.Num() <= 0) {
		return 0;
	}

	// finds longest column size
	int32 longestDroneCol = findLargestColumnSize();

	// select random column from the set of longest columns
	TArray<int32> largestColIndexes;

	for (int32 i = 0; i < ColumnCounts.Num(); ++i) {
		if (ColumnCounts[i] == longestDroneCol) {
			largestColIndexes.Add(i);
		}
	}

	// select random
	int32 randIdx = largestColIndexes[FMath::RandRange(0, largestColIndexes.Num() - 1)];

	// traverse indices, till we find it
	for (auto idx : Index) {
		if (idx.Column == randIdx){
			return idx.Marker->RelativeLocation.Y;
		}
	}

	// this should never happen, but compiler complains
	return 0;
}

void ADroneFormationBase::broadcastDroneSpawn(){
	if (toBeSpawned.Num() > 0) {

		OnFreeSlotAvailable.Broadcast(toBeSpawned[0]);
		toBeSpawned.RemoveAt(0);
	}
}