// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "TentacleBase.h"
#include "CustomExtensions.h"

// Sets default values
ATentacleBase::ATentacleBase() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = root;
}

// Called when the game starts or when spawned
void ATentacleBase::BeginPlay(){
	Super::BeginPlay();
}

// Called every frame
void ATentacleBase::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );
}


void ATentacleBase::BuildTentacle() {

	// clean out existing geometry
	if (Path) {
		Path->DestroyComponent();
	} 
	for (int i = 0; i < Segments.Num(); ++i) {
		Segments[i]->DestroyComponent();
	}
	this->Segments.Empty();


	// build new geometry
	auto path = NewObject<USplineComponent>(this);
	path->ClearSplinePoints();
	path->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	path->SetMobility(EComponentMobility::Movable);
	path->RegisterComponent();

	Path = path;

	if (this->Path && TentacleBodyTemplate) { // make sure we got basics set up
		
		Path->ClearSplinePoints();

		// add all the path points
		for (int i = 0; i < SplinePointCount; ++i) {
			this->Path->AddSplinePoint(GetPointCoord(i),ESplineCoordinateSpace::Local);
		}

		// if there are less than two points, we can't have segments
		if (Path->GetNumberOfSplinePoints() < 2 ) {
			return;
		}

		// create all the mesh segments needed
		auto numOfSegments = Path->GetNumberOfSplinePoints()-1;
		for (int i = 0; i < numOfSegments; ++i) {

			//auto segment = ConstructObject<USplineMeshComponent>(TentacleBodyTemplate->StaticClass(), this);
			auto segment = NewObject<USplineMeshComponent>(this);
			segment->AttachToComponent(path, FAttachmentTransformRules::SnapToTargetIncludingScale);
			segment->SetMobility(EComponentMobility::Movable);
			segment->SetStaticMesh(TentacleBodyTemplate);
			segment->SetForwardAxis(ESplineMeshAxis::Z);
			segment->RegisterComponent();
			this->Segments.Add(segment);
		}

		for (int i = 0; i < Segments.Num(); ++i) {
			// format the segment
			UpdateSegment(i);
		}
	}
}


FVector ATentacleBase::GetPointCoord(int32 idx) {

	// map index to 0 _ 1 range, used as time in Curve component mapping
	float mappedIdx = FMath::GetMappedRangeValueClamped(FVector2D(0, this->SplinePointCount - 1), FVector2D(0, 1), idx);

	// define source ranges Point Start to Point End
	FVector2D xRange = FVector2D(Start.X, End.X);
	FVector2D yRange = FVector2D(Start.Y, End.Y);
	FVector2D zRange = FVector2D(Start.Z, End.Z);
	FVector2D nRange = FVector2D(0, 1);

	// get vector multiplier
	auto vM = Trajectory->GetVectorValue(mappedIdx)*Scale;

	// map each vector values to start - end range
	float mappedX = FMath::GetMappedRangeValueClamped(nRange, xRange, mappedIdx);
	float mappedY = FMath::GetMappedRangeValueClamped(nRange, yRange, mappedIdx);
	float mappedZ = FMath::GetMappedRangeValueClamped(nRange, zRange, mappedIdx);

	// return mapped point + offset derived from vector curve asset
	return FVector(mappedX + vM.X, mappedY + vM.Y, mappedZ + vM.Z);

}

float ATentacleBase::distByFill(float range) {
	auto len = Path->GetSplineLength();
	return FMath::GetMappedRangeValueClamped( FVector2D(0, 1), FVector2D(0, len), FMath::Clamp<float>(range,0,1) );
}

int32 ATentacleBase::findPathSegmentIdxByFill() {

	auto count = Path->GetNumberOfSplinePoints();
	auto len = Path->GetSplineLength();
	auto fillDist = distByFill(len);

	// handle extremes (if fill is at beginning or end)
	if (fillDist <= 0) {
		return 0;
	} else if (fillDist >= len) {
		return count - 1;
	}

	// find the index
	for (int i = 0; i < count; ++i) {
		auto dist = Path->GetDistanceAlongSplineAtSplinePoint(i);
		if (fillDist > dist) {
			continue;
		} else {
			return i-1;
		}
	}

	return 0;
}


bool ATentacleBase::UpdateSegment(int32 idx) {

	// sanity check - make sure we aren't running over segments
	if ( !Segments.Num() || Segments.Num() < idx-1 ) { 
		return false;
	}

	// sanity check - make sure segment exists
	auto segment = Segments[idx];
	if (!segment) {
		return false;
	}
	segment->SetForwardAxis(ESplineMeshAxis::Z);

	// sanity check - make sure two points between which we can make a mesh segment are available
	if ((idx + 1) > Path->GetNumberOfSplinePoints() - 1) {
		return false;
	}

	auto pathLen = Path->GetSplineLength();

	// get segment range beginning
	auto segmentStart = Path->GetDistanceAlongSplineAtSplinePoint(idx);

	// get segment range end
	auto segmentEnd = Path->GetDistanceAlongSplineAtSplinePoint(idx + 1);

	//  get start position of filled path
	auto startPos = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(0, pathLen), StartFill);

	// get end position of filled path
	auto endPos = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(0, pathLen), EndFill);

	float meshStart = 0;
	float meshEnd = 0;

	segment->SetHiddenInGame(false);
	segment->SetVisibility(true, false);
	
	// if tails falls into this index, start mesh at tail position (startPos)
	if (UCustomExtensions::IsInRange(startPos, segmentStart, segmentEnd)) {
		meshStart = startPos;
		
	// if tentacle starts after this segment ends, there is no need to render this segment
	} else if(startPos > segmentEnd) {
		segment->SetHiddenInGame(true);
		segment->SetVisibility(false, false);
		return true;

	} else { // standard segment
		meshStart = segmentStart;
	}

	// if head falls into this index, start mesh at idx+1 position
	if (UCustomExtensions::IsInRange(endPos, segmentStart, segmentEnd)) {
		meshEnd = endPos;

	// if tentacle ends before this segment, there is no need to render this segment
	} else if(endPos < segmentStart) {
		segment->SetHiddenInGame(true);
		segment->SetVisibility(false, false);
		return true;

	} else { // standard segment
		meshEnd = segmentEnd;
	}
	

	// format mesh geometry

	segment->SetStartPosition(Path->GetLocationAtDistanceAlongSpline(meshStart, ESplineCoordinateSpace::Local), false);
	segment->SetStartTangent(Path->GetTangentAtDistanceAlongSpline(meshStart, ESplineCoordinateSpace::Local), false);
	segment->SetStartRoll(Path->GetRollAtDistanceAlongSpline(meshStart, ESplineCoordinateSpace::Local), false);
	auto scaleStart = Path->GetScaleAtDistanceAlongSpline(meshStart);
	FVector2D scaleStart2D = FVector2D(scaleStart.X, scaleStart.Y);
	segment->SetStartScale(scaleStart2D, false);

	segment->SetEndPosition(Path->GetLocationAtDistanceAlongSpline(meshEnd, ESplineCoordinateSpace::Local), false);
	segment->SetEndTangent(Path->GetTangentAtDistanceAlongSpline(meshEnd, ESplineCoordinateSpace::Local), false);
	segment->SetEndRoll(Path->GetRollAtDistanceAlongSpline(meshEnd, ESplineCoordinateSpace::Local), false);
	auto scaleEnd = Path->GetScaleAtDistanceAlongSpline(meshEnd);
	FVector2D scaleEnd2D = FVector2D(scaleEnd.X, scaleEnd.Y);
	segment->SetEndScale(scaleEnd2D, false);

	return true;
}