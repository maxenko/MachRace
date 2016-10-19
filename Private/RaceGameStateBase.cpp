// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceGameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RacePlayerBase.h"
#include "CustomExtensions.h"


ARaceGameStateBase::ARaceGameStateBase() {
	PrimaryActorTick.bCanEverTick = true;
	Stage = GameStage::Prelaunch;
	SetAtmosphereByStage();
}

void ARaceGameStateBase::SetStage(GameStage newStage, bool force) {
	PreviousStage = Stage;
	Stage = newStage;

	if (force) {
		if (newStage == GameStage::DesertBoss) {
			// force to 2600
			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);
			if (shipOk && ship->GetSpeed() < 2600) {
				ship->SetShipSpeed(2600);
			}

			DisableAccelerators =
			DisableDecorativeGeometry =
			DisableObstacles = true;
			

		} else if(newStage == GameStage::InfiniteHex){
			// force 2600 + set lvl1 boss death to true

			Level1BossDefeated = true;

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);
			if (shipOk && ship->GetSpeed() < 2600) {
				ship->SetShipSpeed(2600);
			}

			OnDeathLevel1Boss.Broadcast();
		}
	}
}

void  ARaceGameStateBase::SetLevelOneBossDeafeated() {
	Level1BossDefeated = true;
	OnDeathLevel1Boss.Broadcast();

	SetStage(GameStage::InfiniteHex);
}

// Called when the game starts or when spawned
void ARaceGameStateBase::BeginPlay() {
	Super::BeginPlay();
}

void ARaceGameStateBase::MaintainState() {
	
	auto shipOk = false;
	auto ship = GetRaceShip(shipOk);

	if (!shipOk) {
		return;
	}

	float speed = ship->GetTheoreticalSpeed();

	// level 1  rules
	if (Stage == GameStage::Desert && speed >= 2600 && !Level1BossDefeated) {
		PreviousStage = GameStage::Desert;
		Stage = GameStage::DesertBoss;
		OnSpawnLevel1Boss.Broadcast();
	}
	
}

// Called every frame
void ARaceGameStateBase::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);

	// trigger stage change updated settings
	if (previousStage != Stage) {
		previousStage = Stage;

		// things that change on stage change
		SetAtmosphereByStage();
	}

	UpdateAtmosphereSettings();

	// update speed if needed
	bool shipOk = false;
	auto ship = GetRaceShip(shipOk);
	if (shipOk) {
		float speed = ship->GetTheoreticalSpeed();
		if (speed > TopSpeed) {
			TopSpeed = speed;
		}
	}

	MaintainState();
}

void ARaceGameStateBase::AddIgnoredByLaserTrace(AActor* actorToIgnore) {
	IgnoredByLaserTrace.Add(actorToIgnore);	
}


TArray<AActor*> ARaceGameStateBase::GetActorsIgnoredByLaserTrace(bool doCleanUp) {

	if (doCleanUp) {
		IgnoredByLaserTrace.RemoveAll([](const AActor* a) {
			return !UKismetSystemLibrary::IsValid(a);
		});
	}

	return IgnoredByLaserTrace;
}

void ARaceGameStateBase::ResetXYZGrid(float xOffset) {
	auto w = GetWorld();
	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (pawn) {
		FVector loc = pawn->GetActorLocation();
		FVector newLoc = FVector(loc.X+xOffset, loc.Y, loc.Z);
		w->SetNewWorldOrigin(FIntVector(newLoc.X, newLoc.Y, newLoc.Z)+w->OriginLocation);
		//w->bOriginOffsetThisFrame = true;
	}
}

ARaceContentInstancer* ARaceGameStateBase::GetInstancer(bool& success) {
	if (!Instancer) {
		
		// try to find one in the level
		TArray<AActor*> foundInstancers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaceContentInstancer::StaticClass(), foundInstancers);

		if (foundInstancers.Num() > 0) {

			// grab first instance and reference it.
			Instancer = Cast<ARaceContentInstancer>(foundInstancers[0]);
			success = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Instancer Found."));
			return Instancer;

		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Instancer not found! Game might not work correctly. You need Instancer actor placed anywhere in the level."));
			success = false;
			return NULL;
		}
	}

	success = true;
	return Instancer;
}


ARacePlayerBase* ARaceGameStateBase::GetRacePlayer(bool& success) {
	if (!Player) {

		// try to find one in the level
		TArray<AActor*> foundPlayers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARacePlayerBase::StaticClass(), foundPlayers);

		if (foundPlayers.Num() > 0) {

			// grab first instance and reference it.
			Player = Cast<ARacePlayerBase>(foundPlayers[0]);
			success = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Found."));
			return Player;

		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player not found! Game will not work correctly. Check game mode, and make sure RacePlayer is set as player."));
			success = false;
			return NULL;
		}
	}

	success = true;
	return Player;
}


AExponentialHeightFog* ARaceGameStateBase::GetExponentialFog(bool& success) {
	if (!ExponentialFog) {

		// try to find one in the level
		TArray<AActor*> foundFog;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExponentialHeightFog::StaticClass(), foundFog);

		if (foundFog.Num() > 0) {

			// grab first instance and reference it.
			ExponentialFog = Cast<AExponentialHeightFog>(foundFog[0]);
			success = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Exponential fog component found."));
			return ExponentialFog;

		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Exponential fog component not found! Game might not work correctly. You need Instancer actor placed anywhere in the level."));
			success = false;
			return NULL;
		}
	}

	success = true;
	return ExponentialFog;
}

ARaceShipBase* ARaceGameStateBase::GetRaceShip(bool& success) {
	bool playerOK = false;
	auto player = GetRacePlayer(playerOK);

	if (playerOK) {
		auto ship = player->GetRaceShip(success);
		return ship;
	} else {
		success = false;
		return NULL;
	}
}

int32 ARaceGameStateBase::GetNextDesertTileN(bool increment) {
	auto next = Level1Index + 1;
	auto n = next % 3;

	if(increment) {
		Level1Index++;
	}

	return n;
}

FCameraSettings ARaceGameStateBase::GetCameraSettings(float speed) {

	FCameraSettings settings;

	settings.CameraT = FTransform(
		FRotator(-30, -180, 0),
		FVector(500, 0, 500),
		FVector(1, 1, 1)
	);

	settings.InterpSpeed = 1;
	settings.HudScale = 1;

	// PRELAUNCH
	if (Stage == GameStage::Prelaunch) {

		settings.HudScale = .9;
		settings.Fov = 120;

		return settings;

		// DESERT LEVEL 1
	} else if (Stage == GameStage::Desert) {

		settings.InterpSpeed = 2;
		settings.HudScale = .9;
		settings.Fov = 110;
		settings.CameraT.SetTranslation(FVector(200, 0, 150));

		return settings;

		// DESERT LEVEL 1 BOSS
	} else if (Stage == GameStage::DesertBoss) {

		settings.InterpSpeed = 1;
		settings.HudScale = 1.2;
		settings.Fov = 120;
		settings.CameraT.SetTranslation(FVector(230, 0, 150));

		return settings;

		// INFINITE HEX (BEGINNING)
	} else if (Stage == GameStage::InfiniteHex) {

		settings.InterpSpeed = .2;
		settings.Fov = 120;
		settings.HudScale = 1.3;
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

void ARaceGameStateBase::SetAtmosphereByStage() {
	auto settings = FAtmosphereSettings();

	// update atmosphere
	if (Stage == GameStage::Prelaunch || Stage == GameStage::Desert || Stage == GameStage::DesertBoss) {

		settings.FogDensity = 2.1;
		settings.FogHeightFaloff = 2;
		settings.FogMaxOpacity = 1;
		settings.FogStartDist = 4100;
		settings.FogInscatteringExponent = 128;
		settings.FogInscatteringStartDist = 200;

		settings.FogInscatteringColor = FLinearColor(1, .98, .74, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.172, .23, .25, 1);

	} else if (Stage == GameStage::InfiniteHex) {

		settings.FogDensity = .23;
		settings.FogHeightFaloff = .0001;
		settings.FogMaxOpacity = 1;
		settings.FogStartDist = 1850;
		settings.FogInscatteringExponent = 10;
		settings.FogInscatteringStartDist = 1900;

		settings.FogInscatteringColor = FLinearColor(.447, .638, 1, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.25, .25, .125, 1);
	}

	atmosphereSettings = settings;
}

void ARaceGameStateBase::UpdateAtmosphereSettings() {
	
	if (!ExponentialFog) {
		return;
	}

	// update atmosphere
	if (Stage == GameStage::Prelaunch || Stage == GameStage::Desert || Stage == GameStage::DesertBoss) {
		ARaceGameStateBase::transitionAtmosphere(ExponentialFog, atmosphereSettings, 2);
	} else if (Stage == GameStage::InfiniteHex) {
		ARaceGameStateBase::transitionAtmosphere(ExponentialFog, atmosphereSettings, .07);
	}
}

void ARaceGameStateBase::transitionAtmosphere(AExponentialHeightFog* fog, FAtmosphereSettings to, float speed) {

	auto world = GetWorld();

	if (!world) {
		return;
	}

	auto fogC = fog->GetComponent();
	auto delta = world->GetDeltaSeconds();

	fogC->SetFogDensity(FMath::FInterpTo(fogC->FogDensity, to.FogDensity, delta, speed));
	fogC->SetFogHeightFalloff(FMath::FInterpTo(fogC->FogHeightFalloff, to.FogHeightFaloff, delta, speed));
	fogC->SetFogMaxOpacity(FMath::FInterpTo(fogC->FogMaxOpacity, to.FogMaxOpacity, delta, speed));
	fogC->SetStartDistance(FMath::FInterpTo(fogC->StartDistance, to.FogStartDist, delta, speed));
	fogC->SetDirectionalInscatteringExponent(FMath::FInterpTo(fogC->DirectionalInscatteringExponent, to.FogInscatteringExponent, delta, speed));
	fogC->SetStartDistance(FMath::FInterpTo(fogC->StartDistance, to.FogStartDist, delta, speed));
	fogC->SetDirectionalInscatteringStartDistance(FMath::FInterpTo(fogC->DirectionalInscatteringStartDistance, to.FogInscatteringStartDist, delta, speed));
	fogC->SetFogInscatteringColor(UCustomExtensions::InterpLinearColor(fogC->FogInscatteringColor, to.FogInscatteringColor, delta, speed));
	fogC->SetDirectionalInscatteringColor(UCustomExtensions::InterpLinearColor(fogC->DirectionalInscatteringColor, to.FogInscatteringColorDirectional, delta, speed));

}

// Returns control settings based on Stage of the game and current speed.
FControlSettings ARaceGameStateBase::GetControlSettings(float speed) {

	FControlSettings settings;

	if (Stage == GameStage::Prelaunch) {

		settings.BankingRotationImpulse			= 0;
		settings.BankingRotationImpulseMax		= 0;
		settings.BankingSpeedImpulse			= 0;
		settings.LeapImpulse					= 0;
		settings.MaxBankingSpeed				= 0;

		return settings;

	} else if (Stage == GameStage::Desert) {

		if (speed > -1000) {

			settings.BankingRotationImpulse		= .5;
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

	} else if (Stage == GameStage::DesertBoss) {

		settings.BankingRotationImpulse			= .05;
		settings.BankingRotationImpulseMax		= 20;
		settings.BankingSpeedImpulse			= 290;
		settings.LeapImpulse					= 5000;
		settings.MaxBankingSpeed				= 1000;

		return settings;

	} else if (Stage == GameStage::InfiniteHex) {

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

FUnderfadeSettings ARaceGameStateBase::GetUnderFadeSettings() {

	FUnderfadeSettings settings;

	if (Stage == GameStage::Desert || Stage == GameStage::DesertBoss || Stage == GameStage::Prelaunch) {

		settings.Alpha = 0;
		settings.Scale = FVector(10, 10, 10);
		settings.Radius = .5;
		settings.Color = FLinearColor(0, 0, 0, 0);

		return settings;

	} else if (Stage == GameStage::InfiniteHex) {

		settings.Alpha = 1.5;
		settings.Scale = FVector(6.5, 11, 6.5);
		settings.Radius = .5;
		settings.Color = FLinearColor(.289, .375, .362, 1);

		return settings;
	}

	return settings;
}