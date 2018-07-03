// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceLaser.h"
#include "DrawDebugHelpers.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"


ARaceLaser::ARaceLaser() {

	bIgnoresOriginShifting = false;
	PrimaryActorTick.bCanEverTick = true;
}


void ARaceLaser::buildBeam() {

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

void ARaceLaser::updateBeam() {

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

bool ARaceLaser::isAutoAimScanDue() {

	auto sinceLastTime = FDateTime::Now() - lastAutoAimTraceTime;
	if (AutoAimScanInterval > sinceLastTime.GetTotalMilliseconds()) {
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// handle autoaim logic (sphere trace, looking for closest RaceActorBase with .IsAutomAimTarget == true)
//////////////////////////////////////////////////////////////////////////
bool ARaceLaser::doAutoAimTrace() {

	lastAutoAimTraceTime = FDateTime::Now();

	auto w = GetWorld();
	if (!w) {
		return false;
	}

	auto state = GetState();
	if (!state) {
		return false;
	}

	// reset last found object, if the same target is still relevant, it will pop up on scan again
	autoAimTarget = NULL;

	// do a sphere sweep forward, if anything hits continue!
	FHitResult firstClosestHit;
	FCollisionObjectQueryParams objParams;
	FCollisionQueryParams params;
	params.AddIgnoredActors(state->IgnoredByLaserTrace);
	FCollisionShape shape;
	shape.SetSphere(AutoAimRadius);

	TArray<FHitResult> multiHits;

	bool sphereSweepHit = w->SweepSingleByObjectType(firstClosestHit, From, To, FQuat::Identity, objParams, shape, params);
	//bool sphereSweepHits = w->SweepMultiByObjectType(multiHits, From, To, FQuat::Identity, objParams, shape, params);

	/*

	if (!sphereSweepHits) {
		return false;
	}

	
	for (auto h : multiHits) {

		auto a = h.GetActor();
		if (a->IsValidLowLevel()) {
			if (a->ActorHasTag("IgnoredByLaser")) {
				continue;
			} else if( a->IsA(ARaceActorBase::StaticClass()) ) {

				ARaceActorBase* target = Cast<ARaceActorBase>(a);
				if (target->IsAutoAimTarget) {
					autoAimTarget = target;
					break;
					return true;
				}
			}
		}

		if (ShowDebug) {
			DrawDebugPoint(w, firstClosestHit.ImpactPoint, 50, FColor::White, false, .16, 0);
		}
	}*/

	
	if (!sphereSweepHit) {
		return false;

	} else {

		// are we hitting a target that can be autoaimed?
		if (firstClosestHit.GetActor()->IsA(ARaceActorBase::StaticClass())) {

			ARaceActorBase* target = Cast<ARaceActorBase>(firstClosestHit.GetActor());
			if (!target->IsAutoAimTarget) {
				return false;
			}

			// we must be if we're here
			autoAimTarget = target;
			lastAutoAimHit = firstClosestHit;

			if (ShowDebug) {
				DrawDebugPoint(w, firstClosestHit.ImpactPoint, 50, FColor::White, false, .16, 0);
			}

			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// always tracing ahead, regardless if laser is firing or not
// mainly for on/off states between firing as laser cools off and turns on
//////////////////////////////////////////////////////////////////////////
bool ARaceLaser::traceAhead() {

	auto w = GetWorld();
	if (!w) {
		return false;
	}

	auto state = GetState();
	if (!state) {
		return false;
	}

	FHitResult hit;
	ECollisionChannel channel = ECollisionChannel::ECC_Visibility;

	auto actorLoc = GetActorLocation();
	From = FromMarker ? FromMarker->GetComponentLocation() : actorLoc; // figure out where laser is originating from
	To = From + Direction;
	bool block = false; // whether trace hit anything or not
	bool isAutoAimedTrace = false; // whether or not this trace is hitting auto-aim target. For instance its isn't when player is just shooting straight and hitting actors not marked as 'IsAutoaimTarget.


	//////////////////////////////////////////////////////////////////////////
	// handle auto-aim logic if auto-aim is on on this level
	//////////////////////////////////////////////////////////////////////////
	if (state->EnableAutoAim) {

		// do we have autoAimTarget? If so thats what we need to hit!
		// here we set hit only if it passes all the requirements, otherwise hit is set later to standard straight trace
		if (doAutoAimTrace() && autoAimTarget != NULL) {

			// do a test line trace to the center of that target
			FHitResult testHit;
			bool test = w->LineTraceSingleByChannel(testHit, From, To, channel);

			// is it within cone of lasers vision?
			auto hitVector = (From - lastAutoAimHit.Location);
			auto straightVector = (From + Direction);

			if (UX::VectorsWithinAngle(hitVector, straightVector, LaserConeOfVision)) {
				hit = lastAutoAimHit;
				block = true;
				To = lastAutoAimHit.ImpactPoint;
				isAutoAimedTrace = true;
			}
		}

	//////////////////////////////////////////////////////////////////////////
	// handle standard (aim straight) logic, this runs by default if auto-aim is off or there was no block(ing) hit with auto-aim
	//////////////////////////////////////////////////////////////////////////
	}
	else if (!block) {
		block = w->LineTraceSingleByChannel(hit, From, To, channel);
	}


	if (ShowDebug) {

		FColor traceColor = FColor::Green;
		if (IsFiring) {
			traceColor = FColor::Red;
		}
		DrawDebugLine(w, From, To, traceColor, false, 0.0, 0, 15);
	}

	if (IsFiring == false && previousIsFiring == true) { // prevents double triggering laser from different keys

		EndFiring.Broadcast();

	}
	else if (IsFiring == true && previousIsFiring == false) { // prevents double triggering laser from different keys

		StartFiring.Broadcast();
	}

	previousIsFiring = IsFiring;

	if (block && IsFiring) { // we hit something while firing!

		LastHit = hit;
		HasHit.Broadcast(hit); // broadcast hit, only when firing and hitting

		// we only update .To here if its not auto-aim hit. Otherwise hit should be from previously executed sphere trace.
		if (!isAutoAimedTrace) {
			To = hit.Location;
		}
		isHitting = true;

	}
	else {

		if (isHitting == true) {
			isHitting = false;
			HitEnded.Broadcast();
		}

		if (IsFiring) {
			NoHit.Broadcast();
		}
	}

	return block;
}


// Called when the game starts or when spawned
void ARaceLaser::BeginPlay() {
	Super::BeginPlay();
}


// Called every frame
void ARaceLaser::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// always trace
	traceAhead();

	if (!BeamPath) {
		buildBeam();
	}
	else if (IsFiring) {
		updateBeam();
	}

	if (beamMesh) {
		beamMesh->SetVisibility(IsFiring);
	}

	// update look at rotation between From and To (as it may shift), useful for various effects on the laser to align.
	LaserLookAtRot = UKismetMathLibrary::FindLookAtRotation(From, To);
}


float ARaceLaser::GetCalculatedDamageAmount(float effectiveRange, float falloff, float damage, float distance) {

	float fullDamageDist = FMath::Clamp(effectiveRange - falloff, 0.0f, TNumericLimits<float>::Max());

	// if within effective range, do full damage
	if (distance <= fullDamageDist) {
		return damage;
	}

	// if inside fall-off range, calculate linear decay
	if ( distance > fullDamageDist && distance < effectiveRange ) {
		return FMath::GetRangePct(fullDamageDist, effectiveRange, distance)*damage;
	}

	// out of range;
	return 0.0f;
}