// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MenuCameraPawn.h"


// Sets default values
AMenuCameraPawn::AMenuCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMenuCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMenuCameraPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMenuCameraPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

}

