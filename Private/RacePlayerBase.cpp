// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RacePlayerBase.h"

#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARacePlayerBase::ARacePlayerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARacePlayerBase::BeginPlay() {
	Super::BeginPlay();
	/*
	bool success = false;
	auto ship = this->GetRaceShip(success);
	this->TetherTo(ship);
	
	*/
}

// Called every frame
void ARacePlayerBase::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );

	if (IsTethered) {

		this->trackShip();

	}
}

void ARacePlayerBase::UpdateCamera(UCameraComponent* c, UStaticMeshComponent* hud, FTransform t, float fov, float hudScaleMultiplier, float speed) {

	if (!c) {
		return;
	}

	auto delta = GetWorld()->GetDeltaSeconds();
	auto iT = UKismetMathLibrary::TInterpTo(c->GetRelativeTransform(), t, delta, speed);
	auto iFov = FMath::FInterpTo(c->FieldOfView, fov, delta, speed);

	auto currentHudScale = hud->GetRelativeTransform().GetScale3D();
	auto iHudScale = FMath::VInterpTo(currentHudScale, currentHudScale*hudScaleMultiplier, delta, speed);

	c->SetFieldOfView(iFov);
	c->SetRelativeTransform(iT);
	hud->SetRelativeTransform(
		FTransform(
			hud->GetRelativeTransform().GetRotation(),
			hud->GetRelativeTransform().GetTranslation(),
			iHudScale
		)
	);

}

ARaceShipBase* ARacePlayerBase::GetRaceShip(bool& success) {

	if (!RaceShip) {

		// try to find one in the level
		TArray<AActor*> foundShips;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaceShipBase::StaticClass(), foundShips);

		if (foundShips.Num() > 0) {
			RaceShip = Cast<ARaceShipBase>(foundShips[0]);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("RaceShip instance found."));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RaceShip not found! Game might not work correctly. You need RaceShip actor placed anywhere in the level."));
			success = false;
			return NULL;
		}
	}

	success = true;
	return RaceShip;
}

bool ARacePlayerBase::TetherTo(ARaceShipBase* ship) {
	if (ship) {
		this->RaceShip = ship;
		IsTethered = true;
		this->OnThethered.Broadcast(ship);
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RaceShip not tethered to."));
		IsTethered = false;
	}

	return IsTethered;
}

void ARacePlayerBase::recordFloorTraceToHistory() {

	// keep history if tethered to a ship
	if (IsTethered) {

		if (followHistory.Num() >= FollowHistorySize) {
			followHistory.RemoveAt(FollowHistorySize - 1, 1);
		}

		followHistory.Add(GetActorLocation());
	}
}

void ARacePlayerBase::trackShip() {

	// record player (this) history, we use it to smooth out motion when tracking ship

	auto shipLoc = RaceShip->GetActorLocation();
	auto softLoc = UKismetMathLibrary::GetVectorArrayAverage(followHistory);

	this->SetActorLocation(FVector(shipLoc.X, shipLoc.Y, softLoc.Z));
}