// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MenuV4PlayerControllerBase.h"



// Sets default values for this component's properties
AMenuV4PlayerControllerBase::AMenuV4PlayerControllerBase() : APlayerController() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;
	//bWantsInitializeComponent = true;
}



void AMenuV4PlayerControllerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	

	GetMousePosition(mouseX, mouseY);

	if (previousMouseX != mouseX || previousMouseY != mouseY) {
		OnMouseMove.Broadcast(mouseX, mouseY);
		previousMouseX = mouseX;
		previousMouseY = mouseY;
	}

}