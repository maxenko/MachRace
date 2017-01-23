// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceContentInstancer.h"
#include "RaceActorBase.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ARaceContentInstancer::ARaceContentInstancer() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	InstStandardHex  = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StandardHex"));

	bIgnoresOriginShifting = false;
}


// Called when the game starts or when spawned
void ARaceContentInstancer::BeginPlay() {
	Super::BeginPlay();	
}

void ARaceContentInstancer::Tick( float DeltaTime ) {

	Super::Tick( DeltaTime );
	
	// process buffered spawns
	if (bufferedDirectives.Num() > 0) {
		
		// run through each directive
		for (FBufferedSpawnLinearDirective& dir : bufferedDirectives) {
			
			// if there are no more transforms in the directive, remove it
			if (dir.Transforms.Num() <= 0) {
				//bufferedDirectives.Remove(dir);
				continue;
			}
			
			int limit = dir.SpawnsPerTick; // keep track of limit of spawns per frame (tick)

			for (int32 n = 0; n < dir.Transforms.Num() && limit > 0; ++n) {
				
				limit--; // reduce limit
				FTransform t = dir.Transforms[n];
				auto w = GetWorld();

				if(w){

					auto owner = dir.Owner ? dir.Owner : this;

					FActorSpawnParameters params;

					//params.bNoCollisionFail = true;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					params.bNoFail = true;
					
					AActor* a = NULL;

					if (dir.Type->IsChildOf(ARaceActorBase::StaticClass())) {

						ARaceActorBase* ab = w->SpawnActor<ARaceActorBase>(
							dir.Type,
							t,
							params
							);

						ab->IgnoredActors = dir.Ignored; // assign any actors spawnee might want to ignore in its traces or other game logic
						a = ab;

					} else {

						a = w->SpawnActor<AActor>(
							dir.Type,
							t,
							params
							);
					}

					if (dir.Owner) {
						dir.Owner->Children.Add(a);
					}
				}
			}
			
			dir.Transforms.RemoveAt(0, dir.SpawnsPerTick, true);
		}
	}
}

void ARaceContentInstancer::CreateInstance(UStaticMesh* mesh, FTransform t) {
	
	if (!mesh) {
		return;
	}

	// creates a new UInstancedStaticMeshWithObjectTrackerComponent with given mesh name
	auto addInstC = [&]() {
		auto c = NewObject<UInstancedStaticMeshComponent>(this, *mesh->GetName());
		c->SetupAttachment(RootComponent);
		c->RegisterComponent();
		c->SetStaticMesh(mesh);
		dictionary.Add(c);
		return c;
	};

	// try to find existing component with same mesh
	for (UInstancedStaticMeshComponent* mc : dictionary) {
		if (mc->StaticMesh == mesh) {
			auto idx = mc->AddInstanceWorldSpace(t);
			return;
		}
	}

	// didn't find existing component for given mesh, make one
	auto c = addInstC();
	auto idx = c->AddInstanceWorldSpace(t);
}

void ARaceContentInstancer::CreateInstanceLocally(UStaticMesh* mesh, FTransform t) {

	if (!mesh) {
		return;
	}

	// creates a new UInstancedStaticMeshWithObjectTrackerComponent with given mesh name
	auto addInstC = [&]() {
		auto c = NewObject<UInstancedStaticMeshComponent>(this, *mesh->GetName());
		c->SetupAttachment(RootComponent);
		c->RegisterComponent();
		c->SetStaticMesh(mesh);
		dictionary.Add(c);
		return c;
	};

	// try to find existing component with same mesh
	for (UInstancedStaticMeshComponent* mc : dictionary) {
		if (mc->StaticMesh == mesh) {
			auto idx = mc->AddInstance(t);
			return;
		}
	}

	// didn't find existing component for given mesh, make one
	auto c = addInstC();
	auto idx = c->AddInstance(t);
}

void ARaceContentInstancer::SpawnBuffered(FBufferedSpawnLinearDirective directive) {
	this->bufferedDirectives.Add(directive);
}