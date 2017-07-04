// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "DroneFormationBase.h"


// Sets default values
ADroneFormationBase::ADroneFormationBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADroneFormationBase::BeginPlay(){
	Super::BeginPlay();
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

	for (auto link : Links) {
		if (link->Drone == NULL) {
			OnFreeSlotAvailable.Broadcast(link);
			break; // one per frame, to throttle drone spawns
		}
	}
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

TArray<FVector> ADroneFormationBase::getFormationGrid(){

	TArray<FVector> grid;
	grid.Reserve(Columns*Rows);

	auto spacingX = (Bounds.X*2)/(Rows-1);		// -1 offsets the fill so coordinates fill entire bounds
	auto spacingY = (Bounds.Y*2)/(Columns-1);	// with zero as start pos and bound*2 as end pos

	for(int32 y = 0; y < Columns; ++y){
		for(int32 x = 0; x < Rows; ++x){

			auto cy = (y*spacingY) - Bounds.Y;
			auto cx = (x*spacingX) - Bounds.X;

			grid.Add(FVector(cx,cy,0));
		}
	}

	return grid;
}

void ADroneFormationBase::realignGrid(){

	// allocate new positions array
	auto previous = Positions;
	
	// resize to new setup
	TArray<UActorComponent*> next;
	next.Reserve(Columns*Rows);

	auto grid = getFormationGrid();

	// sanity check
	if(grid.Num() != Columns*Rows){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, 
			FString::Printf(TEXT("DroneFormation system error, grid sizing mismatch. %i != %i"), next.Num(), grid.Num())
		);
		return;
	}

	// clear old components
	auto components = GetComponentsByTag(USceneComponent::StaticClass(), gridMarkerTagName);
	for (auto r : components) {
		RemoveOwnedComponent(r);
	}

	Positions.Empty();

	// add new positioning tags
	for	( auto v : grid ){
		auto c = NewObject<USceneComponent>(this);
		c->RegisterComponent();
		c->SetRelativeLocation(v);
		c->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		c->ComponentTags.Add(gridMarkerTagName);
		Positions.Add(c);
	}

	relinkDrones();
}

void ADroneFormationBase::relinkDrones() {

	TArray<AActor*> newDrones;
	auto newDroneCount = Columns * Rows;
	newDrones.Reserve(newDroneCount);

	// rebuild links
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
		AActor* reassignedDrone = NULL;

		for (auto n = 0; n < Drones.Num(); ++n) {
			auto droneLoc = Drones[n]->GetActorLocation();
			auto thisDist = FVector::Dist(loc, droneLoc);

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
			OnReassignDrone.Broadcast(reassignedDrone);
		}
	}

	// release any leftover drones from the existing set
	for (auto d : Drones) {
		OnReleaseDrone.Broadcast(d);
	}

	Drones = newDrones;
}

void ADroneFormationBase::LinkDrone(AActor* drone, UDroneToFormationLink* link) {
	link->Drone = drone;
	if (!Drones.Contains(drone)) {
		Drones.Add(drone);
	}
}