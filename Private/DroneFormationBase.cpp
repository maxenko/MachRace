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

	detectAndProcessChanges();

	if (DrawDebug) {
		drawDebug();
	}
}

void ADroneFormationBase::drawDebug() {

	// sanity check
	auto w = GetWorld();
	if (!w) {
		return;
	}

	for ( USceneComponent* p : Positions ){
		auto loc = p->GetComponentLocation() + GetActorLocation();
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
	auto components = GetComponentsByTag(USceneComponent::StaticClass(), gridMarketTagName);
	for (auto r : components) {
		RemoveOwnedComponent(r);
	}

	Positions.Empty();

	// add new positioning tags
	for	( auto v : grid ){
		auto c = NewObject<USceneComponent>(this);
		c->RegisterComponent();
		c->SetRelativeLocation(v);
		c->ComponentTags.Add(gridMarketTagName);
		Positions.Add(c);
	}
}