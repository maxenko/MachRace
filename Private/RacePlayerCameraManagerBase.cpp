// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RacePlayerCameraManagerBase.h"
#include "RaceActorBase.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARacePlayerCameraManagerBase::ARacePlayerCameraManagerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARacePlayerCameraManagerBase::BeginPlay()
{
	Super::BeginPlay();

	// get and set gamestate to local prop
	auto w = GetWorld();
	if (w) {
		State = UX::GetRaceState(w);
		auto settings = State->GetCameraSettings(0);
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ARacePlayerCameraManager : Failed to get ARaceGameStateBase!"));
	}
}

// Called every frame
void ARacePlayerCameraManagerBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

void ARacePlayerCameraManagerBase::UpdateRaceView(UCameraComponent* c, USceneComponent* hud, FTransform t, float fov, FVector targetHudScale, float speed, float bloomIntensity, float bloomThreshold) {

	if (!c) {
		return;
	}

	auto delta = GetWorld()->GetDeltaSeconds();


	/*** fov ***/
	auto iT = UKismetMathLibrary::TInterpTo(c->GetRelativeTransform(), t, delta, speed);
	auto iFov = FMath::FInterpTo(c->FieldOfView, fov, delta, speed);


	/*** intensity ***/
	auto iBloomIntensity = FMath::FInterpTo(c->PostProcessSettings.BloomIntensity, bloomIntensity, delta, speed);
	auto iBloomThreshold = FMath::FInterpTo(c->PostProcessSettings.BloomThreshold, bloomThreshold, delta, speed);

	c->PostProcessSettings.bOverride_BloomIntensity = true;
	c->PostProcessSettings.bOverride_BloomThreshold = true;

	c->PostProcessSettings.BloomIntensity = iBloomIntensity;
	c->PostProcessSettings.BloomThreshold = iBloomThreshold;


	/*** hud ***/
	auto currentHudScale = hud->GetRelativeTransform().GetScale3D();
	auto iHudScale = FMath::VInterpTo(currentHudScale, targetHudScale, delta, speed);

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
