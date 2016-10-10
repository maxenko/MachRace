// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "PawnBase.h"

#include "Kismet/KismetMathLibrary.h"


// Sets default values
APawnBase::APawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APawnBase::UpdateCamera(UCameraComponent* c, UStaticMeshComponent* hud, FTransform t, float fov, float hudScaleMultiplier, float speed) {

	if (!c) {
		return;
	}

	auto delta = GetWorld()->GetDeltaSeconds();
	auto iT = UKismetMathLibrary::TInterpTo(c->GetRelativeTransform(), t, delta, speed);
	auto iFov = UKismetMathLibrary::FInterpTo(c->FieldOfView, fov, delta, speed);

	auto currentHudScale = hud->GetRelativeTransform().GetScale3D();
	auto iHudScale = UKismetMathLibrary::VInterpTo(currentHudScale, currentHudScale*hudScaleMultiplier, delta, speed);

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

// Called when the game starts or when spawned
void APawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawnBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void APawnBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

