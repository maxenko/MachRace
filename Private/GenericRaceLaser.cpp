// Copyright 2015 - Max Enko

#include "GenericRaceLaser.h"
#include "MachRace.h"
#include "DrawDebugHelpers.h"



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

	if (IsFiring) {

		if (!BeamPath) {
			buildBeam();
		}

		if (!previousFireStatus) {
			previousFireStatus = true;
			StartFiring.Broadcast();
		}

		updateBeam();

	} else {
		
		if (previousFireStatus) {
			previousFireStatus = false;
			clearBeam();
			EndFiring.Broadcast();
			
		}
	
	} 

	if (DrawDebug) {
		drawDebug(OnlyWhenFiring);
	}
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

	points.Add(FromMarker->GetComponentLocation());

	if (StopOnBlock) {

		// project between

		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool isAHit = GetWorld()->LineTraceSingleByChannel(hit, FromMarker->GetComponentLocation(), ToMarker->GetComponentLocation(), ECollisionChannel::ECC_Visibility, params);

		if (isAHit) {
			ToMarker->SetWorldLocation(hit.Location);
		}
	}

	points.Add(ToMarker->GetComponentLocation());
	
	BeamPath->SetSplinePoints(points, ESplineCoordinateSpace::World, false);
	BeamPath->UpdateSpline();

	beamMesh->SetStartPosition(BeamPath->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::Local), false);
	beamMesh->SetStartScale(BeamFromScale);


	beamMesh->SetEndPosition(BeamPath->GetLocationAtDistanceAlongSpline(BeamPath->GetSplineLength(), ESplineCoordinateSpace::Local), false);
	beamMesh->SetEndScale(BeamToScale);

	beamMesh->UpdateMesh();
}

void AGenericRaceLaser::clearBeam() {
	BeamPath->ClearSplinePoints(true);
}

void AGenericRaceLaser::SetLaserDestination(FVector toLoc) {
	ToMarker->SetWorldLocation(toLoc);
}

void AGenericRaceLaser::drawDebug(bool onlyIfFiring) {

	auto draw = [](AActor* a, USceneComponent* from, USceneComponent* to) {

		// sanity check
		auto w = a->GetWorld();
		if (!w) {
			return;
		}

		DrawDebugSphere(w, from->GetComponentLocation(), 200, 64, FColor::Blue, false, .08, 0, 7.0);
		DrawDebugSphere(w, to->GetComponentLocation(), 200, 64, FColor::Red, false, .08, 0, 7.0);
	};

	if (onlyIfFiring) {
		if (IsFiring) {
			draw(this,FromMarker,ToMarker);
		}
	} else if (DrawDebug) {
		draw(this, FromMarker, ToMarker);
	}
}