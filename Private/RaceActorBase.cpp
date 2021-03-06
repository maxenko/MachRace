// Copyright 2015 - Max Enko

class ARaceGameStateBase;

#include "RaceActorBase.h"
#include "MachRace.h"
#include "X.h"
#include "RaceGameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ARaceActorBase::ARaceActorBase(){
	PrimaryActorTick.bCanEverTick = true;
	bIgnoresOriginShifting = false;
}

ARaceContentInstancer* ARaceActorBase::GetInstancer(bool& success) {
	auto instancer = GetState()->GetInstancer(success);
	return instancer;
}

void ARaceActorBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
	State = GetState(); // only available in BeginPlay in BP
}

// Called when the game starts or when spawned
void ARaceActorBase::BeginPlay(){
	Super::BeginPlay();
}

// Called every frame
void ARaceActorBase::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );

	if (KillSelfIfBehindCamera) {
		if (GEngine) {  

			auto w = GetWorld();

			if (w) {

				auto flpc = GEngine->GetFirstLocalPlayerController(w);

				if (flpc) {

					auto pcm = flpc->PlayerCameraManager;

					if (pcm) {
						FVector loc = pcm->GetCameraLocation();
						float bufferedThisLocX = GetActorLocation().X - KillSelfIfBehindCameraAdditoonalBufferDistanceX;
						if (loc.X < bufferedThisLocX || FVector::Distance(loc, GetActorLocation()) >= 499999.0) {

							if (IsAPoolActor) {

								this->ReceiveReturnToPool();
								TimesPooled++;

							} else {

								DestroyChildren();
								Destroy();

							}

						}
					}
				}
			}
		}
	}
}

// Called every frame
ARaceGameStateBase* ARaceActorBase::GetState() {
	return UX::GetRaceState(GetWorld());
}


// Called every frame
ARaceGameStateBase* ARaceActorBase::GetStateSafe(bool &success){
	State = UX::GetRaceState(GetWorld());

	if (!State) {
		success = false;
	}
	else {
		success = true;
	}

	return State;
}

void ARaceActorBase::RememberTransform(UObject* obj, FTransform transform) {
	
	// exists?
	for (FObjectTransformRecord t : transformRecord) {
		if (t.Object == obj) {
			t.Transform = transform;

			return;
		}
	}

	// doesn't exist?
	FObjectTransformRecord record;
	record.Object = obj;
	record.Transform = transform;
	transformRecord.Add(record);
}

bool ARaceActorBase::RecallTransform(UObject* obj, FTransform& transform) {

	for (FObjectTransformRecord t : transformRecord) {
		if (t.Object == obj) {
			transform = t.Transform;
			return true;
		}
	}

	transform = FTransform::Identity;
	return false;
}

void ARaceActorBase::AddChild(AActor* a) {
	Children.Add(a);
}

int32 ARaceActorBase::DestroyChildren(bool buffered) {
	auto count = this->Children.Num();

	for (auto a : Children) {
		if (!UKismetSystemLibrary::IsValid(a)) {
			continue;

		}

		if (buffered) {
			a->SetLifeSpan(FMath::RandRange(1,2));
		} else {
			a->Destroy();
		}
	}

	return count;
}