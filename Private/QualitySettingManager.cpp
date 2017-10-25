// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "QualitySettingManager.h"


// Sets default values for this component's properties
UQualitySettingManager::UQualitySettingManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQualitySettingManager::BeginPlay()
{
	Super::BeginPlay();

	loadConfig();

	// ...
	
}

void UQualitySettingManager::loadConfig() {

	if (!GConfig) { return; }

	int32 qualitySetting = -1;

	qualitySetting = UX::GetIntSetting("MachRace.Settings", "QualitySetting");

	if (qualitySetting < 0) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Quality setting not found. Writing default value.");

		UX::StoreIntSetting("MachRace.Settings", "QualitySetting", (int32)CurrentSetting);

	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Quality setting loaded: %s"), *FString::FromInt(qualitySetting)));
	}

}


// Called every frame
void UQualitySettingManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UQualitySettingManager::executeConsoleCommand(FString cmd) {

	APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PController)
	{
		PController->ConsoleCommand(*cmd, true);
	}
}

void UQualitySettingManager::ApplyPreset(QualitySetting qs) {

	// run the console commands
	auto runCommands = [&](TArray<FString> commands) {
		for (FString c : commands) {

			executeConsoleCommand(c.Trim().TrimTrailing());

			if(PrintDebug) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, c.Trim().TrimTrailing());
			}
		}
	};

	// parse unreal console commands, from ; delimited string
	auto parseCommands = [&](FString commands) {
		TArray<FString> parsed;
		commands.ParseIntoArray(parsed, TEXT(";"), true);
		return parsed;
	};

	char* commands; // will hold our commands

	switch (qs) {
		
		case QualitySetting::Potato:
			commands = "\
				r.ScreenPercentage = 80;\
				r.MotionBlurQuality=0;\
				r.BlurGBuffer = 0;\
				r.AmbientOcclusionLevels = 0;\
				r.AmbientOcclusionRadiusScale = 1.7;\
				r.DepthOfFieldQuality = 0;\
				r.RenderTargetPoolMin = 300;\
				r.LensFlareQuality = 0;\
				r.SceneColorFringeQuality = 0;\
				r.EyeAdaptationQuality = 0;\
				r.BloomQuality = 4;\
				r.FastBlurThreshold = 0;\
				r.Upscale.Quality = 1;\
				r.Tonemapper.GrainQuantization = 0;\
				\
				r.LightFunctionQuality = 0;\
				r.ShadowQuality = 0;\
				r.Shadow.CSM.MaxCascades = 1;\
				r.Shadow.MaxResolution = 512;\
				r.Shadow.RadiusThreshold = 0.06;\
				r.Shadow.DistanceScale = 0.6;\
				r.Shadow.CSM.TransitionScale = 0;\
				\
				r.Streaming.MipBias = 2.5;\
				r.MaxAnisotropy = 0;\
				r.Streaming.PoolSize = 200;\
				\
				r.TranslucencyLightingVolumeDim=24;\
				r.RefractionQuality = 0;\
				r.SSR = 0;\
				r.SceneColorFormat = 3;\
				r.DetailMode = 0;\
				r.TranslucencyVolumeBlur = 0;\
				r.MaterialQualityLevel = 0";
			break;

		case QualitySetting::Toaster:
			commands = "\
				r.ScreenPercentage = 90;\
				r.MotionBlurQuality=3;\
				r.BlurGBuffer = 0;\
				r.AmbientOcclusionLevels = 1;\
				r.AmbientOcclusionRadiusScale = 1.7;\
				r.DepthOfFieldQuality = 1;\
				r.RenderTargetPoolMin = 350;\
				r.LensFlareQuality = 0;\
				r.SceneColorFringeQuality = 0;\
				r.EyeAdaptationQuality = 0;\
				r.BloomQuality = 4;\
				r.FastBlurThreshold = 2;\
				r.Upscale.Quality = 2;\
				r.Tonemapper.GrainQuantization = 0;\
				\
				r.LightFunctionQuality = 1;\
				r.ShadowQuality = 2;\
				r.Shadow.CSM.MaxCascades = 1;\
				r.Shadow.MaxResolution = 1024;\
				r.Shadow.RadiusThreshold = 0.05;\
				r.Shadow.DistanceScale = 0.7;\
				r.Shadow.CSM.TransitionScale = 0.25;\
				\
				r.Streaming.MipBias = 1;\
				r.MaxAnisotropy = 2;\
				r.Streaming.PoolSize = 400;\
				\
				r.TranslucencyLightingVolumeDim=32;\
				r.RefractionQuality = 0;\
				r.SSR = 0;\
				r.SceneColorFormat = 3;\
				r.DetailMode = 1;\
				r.TranslucencyVolumeBlur = 0;\
				r.MaterialQualityLevel = 1";
			break;

		case QualitySetting::PCMasterRace:
			commands = "\
				r.ScreenPercentage = 100;\
				r.MotionBlurQuality=3;\
				r.BlurGBuffer = -1;\
				r.AmbientOcclusionLevels = 2;\
				r.AmbientOcclusionRadiusScale = 1.5;\
				r.DepthOfFieldQuality = 2;\
				r.RenderTargetPoolMin = 400;\
				r.LensFlareQuality = 2;\
				r.SceneColorFringeQuality = 1;\
				r.EyeAdaptationQuality = 2;\
				r.BloomQuality = 5;\
				r.FastBlurThreshold = 3;\
				r.Upscale.Quality = 2;\
				r.Tonemapper.GrainQuantization = 1;\
				\
				r.LightFunctionQuality = 1;\
				r.ShadowQuality = 5;\
				r.Shadow.CSM.MaxCascades = 2;\
				r.Shadow.MaxResolution = 1024;\
				r.Shadow.RadiusThreshold = 0.04;\
				r.Shadow.DistanceScale = 0.85;\
				r.Shadow.CSM.TransitionScale = 0.8;\
				\
				r.Streaming.MipBias = 0;\
				r.MaxAnisotropy = 4;\
				r.Streaming.PoolSize = 700;\
				\
				r.TranslucencyLightingVolumeDim = 48;\
				r.RefractionQuality = 2;\
				r.SSR = 0;\
				r.SceneColorFormat = 3;\
				r.DetailMode = 1;\
				r.TranslucencyVolumeBlur = 1;\
				r.MaterialQualityLevel = 1";
			break;

		case QualitySetting::QuantumComputer:
			commands = "\
				r.ScreenPercentage = 100;\
				r.MotionBlurQuality=4;\
				r.BlurGBuffer = -1;\
				r.AmbientOcclusionLevels = 3;\
				r.AmbientOcclusionRadiusScale = 1.0;\
				r.DepthOfFieldQuality = 2;\
				r.RenderTargetPoolMin = 400;\
				r.LensFlareQuality = 2;\
				r.SceneColorFringeQuality = 1;\
				r.EyeAdaptationQuality = 2;\
				r.BloomQuality = 5;\
				r.FastBlurThreshold = 7;\
				r.Upscale.Quality = 3;\
				r.Tonemapper.GrainQuantization = 1;\
				\
				r.LightFunctionQuality=1;\
				r.ShadowQuality = 5;\
				r.Shadow.CSM.MaxCascades = 4;\
				r.Shadow.MaxResolution = 1024;\
				r.Shadow.RadiusThreshold = 0.03;\
				r.Shadow.DistanceScale = 1.0;\
				r.Shadow.CSM.TransitionScale = 1.0;\
				\
				r.Streaming.MipBias=0;\
				r.MaxAnisotropy = 8;\
				r.Streaming.PoolSize = 1000;\
				\
				r.TranslucencyLightingVolumeDim = 64;\
				r.RefractionQuality = 2;\
				r.SSR = 1;\
				r.SceneColorFormat = 4;\
				r.DetailMode = 2;\
				r.TranslucencyVolumeBlur = 1; \
				r.MaterialQualityLevel = 1";
			break;

		default:
			commands = "";
	}

	CurrentSetting = qs;
	runCommands(parseCommands(FString(ANSI_TO_TCHAR(commands))));
}