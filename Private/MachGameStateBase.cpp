// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MachGameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomExtensions.h"


FCameraSettings AMachGameStateBase::GetCameraSettings(GameStage stage, float speed) {

	FCameraSettings settings;

	settings.CameraT = FTransform(
		FRotator(-30, -180, 0),
		FVector(500, 0, 500),
		FVector(1, 1, 1)
	);

	settings.InterpSpeed = 1;
	settings.HudScale = 1;

	// PRELAUNCH
	if (stage == GameStage::Prelaunch) {
		
		settings.HudScale	= .5;
		settings.HudScale	= 1.3;
		settings.Fov		= 120;

		return settings;
		
	// DESERT LEVEL 1
	} else if (stage == GameStage::Desert) {
		
		settings.InterpSpeed	= 2;
		settings.HudScale		= .5;
		settings.Fov			= 110;
		settings.CameraT.SetTranslation(FVector(200, 0, 150));

		return settings;


	// DESERT LEVEL 1 BOSS
	} else if (stage == GameStage::DesertBoss) {

		settings.InterpSpeed	= 1;
		settings.HudScale		= 1.4;
		settings.Fov			= 120;
		settings.CameraT.SetTranslation(FVector(230, 0, 150));

		return settings;

	// INFINITE HEX (BEGINNING)
	} else if (stage == GameStage::InfiniteHex) {

		settings.InterpSpeed	= .2;
		settings.Fov			= 120;
		settings.HudScale		= 2.7;
		settings.CameraT.SetTranslation(FVector(245, 0, 220));
		settings.CameraT.SetRotation(FRotator(-29, -180, 0).Quaternion());
		
		
		if (speed < 2000) {

			return settings;

		} else if (speed > 2500) {

			settings.CameraT.SetTranslation(FVector(260, 0, 220));
			settings.Fov = 121.5;
		
		// INFINITE HEX (END)
		} else if (speed > 3100) {

			settings.CameraT.SetTranslation(FVector(275, 0, 220));
			settings.CameraT.SetRotation(FRotator(-29, -180, 0).Quaternion());
			settings.Fov = 123;
		}

		return settings;
	
	} 

	return settings;
}

FAtmosphereSettings AMachGameStateBase::UpdateAtmosphereSettings(GameStage stage, AExponentialHeightFog* fog) {
	auto settings = FAtmosphereSettings();

	// update atmosphere
	if (stage == GameStage::Prelaunch || stage == GameStage::Desert || stage == GameStage::DesertBoss) {
		
		settings.FogDensity					= 2;
		settings.FogHeightFaloff			= 2;
		settings.FogMaxOpacity				= 1;
		settings.FogStartDist				= 5500;
		settings.FogInscatteringExponent	= 128;
		settings.FogInscatteringStartDist	= 390;

		settings.FogInscatteringColor = FLinearColor(1, .98, .74, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.172,.23,.25,1);

		AMachGameStateBase::transitionAtmosphere(fog, settings, 2);

	} else if (stage == GameStage::InfiniteHex) {

		settings.FogDensity					= .23;
		settings.FogHeightFaloff			= .0001;
		settings.FogMaxOpacity				= 1;
		settings.FogStartDist				= 1850;
		settings.FogInscatteringExponent	= 10;
		settings.FogInscatteringStartDist	= 1900;

		settings.FogInscatteringColor = FLinearColor(.447, .638, 1, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.25, .25, .125, 1);

		AMachGameStateBase::transitionAtmosphere(fog, settings, .07);

	}

	return settings;
}

void AMachGameStateBase::transitionAtmosphere(AExponentialHeightFog* fog, FAtmosphereSettings to, float speed) {

	auto world = GetWorld();

	if (!world) {
		return;
	}

	auto fogC = fog->GetComponent();
	auto delta = world->GetDeltaSeconds();

	fogC->SetFogDensity( FMath::FInterpTo(fogC->FogDensity, to.FogDensity, delta, speed) );
	fogC->SetFogHeightFalloff(FMath::FInterpTo(fogC->FogHeightFalloff, to.FogHeightFaloff, delta, speed));
	fogC->SetFogMaxOpacity(FMath::FInterpTo(fogC->FogMaxOpacity, to.FogMaxOpacity, delta, speed));
	fogC->SetStartDistance(FMath::FInterpTo(fogC->StartDistance, to.FogStartDist, delta, speed));
	fogC->SetDirectionalInscatteringExponent(FMath::FInterpTo(fogC->DirectionalInscatteringExponent, to.FogInscatteringExponent, delta, speed));
	fogC->SetStartDistance(FMath::FInterpTo(fogC->StartDistance, to.FogStartDist, delta, speed));
	fogC->SetDirectionalInscatteringStartDistance(FMath::FInterpTo(fogC->DirectionalInscatteringStartDistance, to.FogInscatteringStartDist, delta, speed));
	
	fogC->SetFogInscatteringColor(UX::InterpLinearColor(fogC->FogInscatteringColor, to.FogInscatteringColor, delta, speed));
	fogC->SetDirectionalInscatteringColor(UX::InterpLinearColor(fogC->DirectionalInscatteringColor, to.FogInscatteringColorDirectional, delta, speed));

}

// Returns control settings based on stage of the game and current speed.
FControlSettings AMachGameStateBase::GetControlSettings(GameStage stage, float speed) {

	FControlSettings settings;
	settings.BankingRotationImpulse = 0;
	settings.BankingRotationImpulseMax = 0;
	settings.BankingSpeedImpulse = 0;
	settings.LeapImpulse = 0;
	settings.MaxBankingSpeed = 0;

	if (stage == GameStage::Prelaunch) {

		return settings;

	} else if (stage == GameStage::Desert) {

		if (speed > -1000) {

			settings.BankingRotationImpulse		= .05;
			settings.BankingRotationImpulseMax	= 20;
			settings.BankingSpeedImpulse		= 250;
			settings.LeapImpulse				= 4000;
			settings.MaxBankingSpeed			= 900;

		} else if (speed >= 1000) {

			settings.BankingRotationImpulse		= .05;
			settings.BankingRotationImpulseMax	= 20;
			settings.BankingSpeedImpulse		= 290;
			settings.LeapImpulse				= 4500;
			settings.MaxBankingSpeed			= 1000;

		}

		return settings;

	} else if (stage == GameStage::DesertBoss) {

		settings.BankingRotationImpulse			= .05;
		settings.BankingRotationImpulseMax		= 20;
		settings.BankingSpeedImpulse			= 290;
		settings.LeapImpulse					= 5000;
		settings.MaxBankingSpeed				= 1000;

		return settings;

	} else if (stage == GameStage::InfiniteHex) {

		settings.BankingRotationImpulse			= .05;
		settings.BankingRotationImpulseMax		= 20;
		settings.BankingSpeedImpulse			= 450;
		settings.LeapImpulse					= 8000;
		settings.MaxBankingSpeed				= 1500;

		return settings;

	} else {

		settings.BankingRotationImpulse			= .45;
		settings.BankingRotationImpulseMax		= 20;
		settings.BankingSpeedImpulse			= 290;
		settings.LeapImpulse					= 5000;
		settings.MaxBankingSpeed				= 1000;

	}

	return settings;

}

FUnderfadeSettings AMachGameStateBase::GetUnderFadeSettings(GameStage stage) {

	FUnderfadeSettings settings;

	if (stage == GameStage::Desert || stage == GameStage::DesertBoss || stage == GameStage::Prelaunch) {

		settings.Alpha	= 0;
		settings.Scale	= FVector(10,10,10);
		settings.Radius = .5;
		settings.Color	= FLinearColor(0, 0, 0, 0);

		return settings;

	} else if (stage == GameStage::InfiniteHex) {

		settings.Alpha	= 1.5;
		settings.Scale	= FVector(6.5, 11, 6.5);
		settings.Radius = .5;
		settings.Color	= FLinearColor(.289, .375, .362, 1);

		return settings;

	}

	return settings;
}