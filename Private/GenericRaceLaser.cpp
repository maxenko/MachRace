// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "GenericRaceLaser.h"


// Sets default values
AGenericRaceLaser::AGenericRaceLaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGenericRaceLaser::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGenericRaceLaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGenericRaceLaser::buildBeam() {

	// we need mesh component before we proceed 
	if (!BeamBodyTemplate) {
		return;
	}

	// build new curve
	auto path = NewObject<USplineComponent>(this, NAME_None);
	path->SetMobility(EComponentMobility::Movable);
	path->ClearSplinePoints();
	path->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	path->RegisterComponent();

	BeamPath = path;

	auto segment = NewObject<USplineMeshComponent>(path);
	segment->SetMobility(EComponentMobility::Movable);
	segment->SetStaticMesh(BeamBodyTemplate);
	segment->SetForwardAxis(ESplineMeshAxis::Z);
	if (BeamMaterial) {
		segment->SetMaterial(0, BeamMaterial);
	}
	segment->AttachToComponent(BeamPath, FAttachmentTransformRules::KeepRelativeTransform);
	segment->RegisterComponent();

	beamMesh = segment;
}

void AGenericRaceLaser::updateBeam() {

	BeamPath->ClearSplinePoints(false);
	TArray<FVector>points;

	points.Add(From);
	points.Add(To);

	BeamPath->SetSplinePoints(points, ESplineCoordinateSpace::World, false);
	BeamPath->UpdateSpline();

	beamMesh->SetStartPosition(BeamPath->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::Local), false);
	beamMesh->SetStartScale(BeamFromScale);

	beamMesh->SetEndPosition(BeamPath->GetLocationAtDistanceAlongSpline(BeamPath->GetSplineLength(), ESplineCoordinateSpace::Local), false);
	beamMesh->SetEndScale(BeamToScale);

	beamMesh->UpdateMesh();
}