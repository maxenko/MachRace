// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "ProjectileBase.h"


// Sets default values
AProjectileBase::AProjectileBase() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay() {
	Super::BeginPlay();

	lifeBeganTime = FDateTime::Now();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// self destruct
	auto now = FDateTime::Now();
	auto lifeSpanInSeconds = (float)(now - lifeBeganTime).GetSeconds();

	if (lifeSpanInSeconds >= this->MaxLifeTime) {
		this->Destroy();
	}

}


bool AProjectileBase::SetProjectileVelocity(FVector velocity) {
	auto root = GetRootComponent();
	auto physicsComponent = Cast<UPrimitiveComponent>(root);

	if (!physicsComponent) {
		return false;
	}

	physicsComponent->SetPhysicsLinearVelocity(velocity);

	return true;
}
