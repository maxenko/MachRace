// Copyright 2015 - Max Enko

#include "MenuV4PlayerControllerBase.h"
#include "MachRace.h"
#include "GameFramework/InputSettings.h"


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

// Called when the game starts
void AMenuV4PlayerControllerBase::BeginPlay() {
	Super::BeginPlay();
}


FInputActionKeyMapping AMenuV4PlayerControllerBase::GetInputActionSettingFor(bool& success, FName name, MachRaceInputType inputType) {

	auto inputSettings = UInputSettings::GetInputSettings();

	TArray<FInputActionKeyMapping> mappings;
	inputSettings->GetActionMappingByName(name, mappings);

	for (int i = 0; i < mappings.Num(); ++i) {

		if (mappings[i].Key.IsGamepadKey() && inputType == MachRaceInputType::JoystickInput) {

			success = true;
			return mappings[i];

		} else if (!mappings[i].Key.IsGamepadKey() && inputType == MachRaceInputType::KeyboardInput) {

			success = true;
			return mappings[i];

		}
	}

	success = false;
	return FInputActionKeyMapping();
}
