// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "ResolutionSettingManager.h"


// Sets default values for this component's properties
UResolutionSettingManager::UResolutionSettingManager() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UResolutionSettingManager::BeginPlay() {
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UResolutionSettingManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


TArray<FString> UResolutionSettingManager::GetSupportedResolutions() {

	// parse unreal console commands, from ; delimited string
	auto parseResolutions = [&](FString delimited) {
		TArray<FString> parsed;
		delimited.ParseIntoArray(parsed, TEXT(";"), true);
		for (int i = 0; i < parsed.Num(); ++i) {
			parsed[i] = parsed[i].Trim().TrimTrailing();
		}
		return parsed;
	};

	// 16:9
	// 21:9

	char* resolutions = "\
		1280x720;\
		1600x900;\
		1920x1080;\
		2560x1440;\
		3840x2160;\
		\
		2560x1080;\
		3440x1440;\
		";

	return parseResolutions(resolutions);
}


FString UResolutionSettingManager::GetConfigResolution(bool& success) {
	FString resolution = UX::GetStringSetting("MachRace.Settings", "Resolution").Trim().TrimTrailing();

	if (resolution.Len() <= 0) {
		success = false;
		return "";
	} else {
		success = true;
		return resolution;	
	}
}