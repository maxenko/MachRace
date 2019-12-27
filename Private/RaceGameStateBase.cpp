 // Copyright 2015 - Max Enko

#include "RaceGameStateBase.h"
#include "MachRace.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RacePlayerBase.h"
#include "X.h"
#include "Engine/Engine.h"
#include "Engine/ExponentialHeightFog.h"
#include "CommonTypes.h"
#include "Components/ExponentialHeightFogComponent.h"


ARaceGameStateBase::ARaceGameStateBase() {
	PrimaryActorTick.bCanEverTick = true;
	Stage = GameStage::Prelaunch;
	SetAtmosphereByStage();
}

// Called when the game starts or when spawned
void ARaceGameStateBase::BeginPlay() {
	Super::BeginPlay();

	// initialize delta history to all 0.0's
	deltaTimeHistory.Init(0, DeltaTimeHistorySize);
}

void ARaceGameStateBase::SetStage(GameStage newStage, bool force) {
	PreviousStage = Stage;
	Stage = newStage;

	// below is a long list of bureaucracy in dealing with dev and testing, this code is not used by players in game.
	// its just here to dev hack into various stages in game
	if (force) {
		if (newStage == GameStage::DesertBoss) {
			// force to Level1BossTriggerSpeed
			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);
			if (shipOk && ship->GetTheoreticalSpeed() < Level1BossTriggerSpeed) {
				ship->SetShipSpeed(Level1BossTriggerSpeed);
			}

			EnableAutoAim =
			DisableAccelerators =
			DisableDecorativeGeometry =
			DisableObstacles = true;

			OnSpawnLevel1Boss.Broadcast();

		} else if (newStage == GameStage::InfiniteHex) {
			// force Level1BossTriggerSpeed + set lvl1 boss death to true

			EnableAutoAim =
				Level1BossDefeated = true;

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);
			if (shipOk && ship->GetTheoreticalSpeed() < Level1BossTriggerSpeed) {
				ship->SetShipSpeed(Level1BossTriggerSpeed);
			}

			SetLevelOneBossDeafeated();

		} else if (newStage == GameStage::InfiniteHexBoss){

			EnableAutoAim =
				Level1BossDefeated = false;

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);
			if (shipOk && ship->GetTheoreticalSpeed() < Level1BossTriggerSpeed) {
				ship->SetShipSpeed(Level2BossTriggerSpeed);
				OnLevel2Boss.Broadcast();
			}

			SetLevelOneBossDeafeated();

		}
		else if (newStage == GameStage::Labyrinth) {

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);

			ship->SetShipSpeed(Level3TriggerSpeed);

			OnLevel3Reached.Broadcast();

			EnableAutoAim = false;
			Level1BossDefeated = true;

			SetLevelOneBossDeafeated();

		} else if (newStage == GameStage::LabyrinthBoss) {

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);

			ship->SetShipSpeed(Level3Stage2TriggerSpeed+1);

			OnLevel3BossReached.Broadcast();

			//EnableAutoAim = true;
			Level1BossDefeated = true;

			SetLevelOneBossDeafeated();

		} else if (newStage == GameStage::Space) {

			bool shipOk = false;
			auto ship = GetRaceShip(shipOk);

			ship->SetShipSpeed(Level4TriggerSpeed);

			EnableAutoAim = false;
			Level1BossDefeated = true;

			SetLevelOneBossDeafeated();

			OnLevel4Reached.Broadcast();
		}
	}

	OnOnStageChange.Broadcast(previousStage, newStage);
}

void  ARaceGameStateBase::SetLevelOneBossDeafeated() {
	Level1BossDefeated = true;
	OnDeathLevel1Boss.Broadcast();
}

void ARaceGameStateBase::MaintainState() {
	
	auto shipOk = false;
	auto ship = GetRaceShip(shipOk);

	if (!shipOk) {
		return;
	}

	float speed = ship->GetTheoreticalSpeed();

	// level 1 rules
	if (Stage == GameStage::Desert && speed >= Level1BossTriggerSpeed && !Level1BossDefeated) {
		SetStage(GameStage::DesertBoss);
		//PreviousStage = GameStage::Desert;
		//Stage = GameStage::DesertBoss;
		OnSpawnLevel1Boss.Broadcast();
		//OnOnStageChange.Broadcast(previousStage, Stage);
		return;
	}


	// level 2 rules
	if (Stage == GameStage::InfiniteHex) {

		if (speed >= Level2BossTriggerSpeed && !Level2BossDefeated) {
			SetStage(GameStage::InfiniteHexBoss);
			OnLevel2Boss.Broadcast();
			return;
		}

		return;

	} else if (Stage == GameStage::InfiniteHexBoss) {

		// did player reached level 3 speed?
		if (speed >= Level3TriggerSpeed && !Level3Disable /* has developer disabled level3? */ ) {

			// set up for level 3

			if (SafeToTransitionToLevel3) {
				SetStage(GameStage::Labyrinth);
				OnLevel3Reached.Broadcast();
				return;
			} else {
				if (!level2BossSetToBeRemoved) {
					OnRemoveLevel2Boss.Broadcast();
					level2BossSetToBeRemoved = true; // do once flag
				}
			}

			return;
		}

	}

	
	if (Stage == GameStage::Labyrinth) {
		if (speed >= Level3Stage2TriggerSpeed && speed < Level4TriggerSpeed) {
			SetStage(GameStage::LabyrinthBoss);
			EnableAutoAim = false;
			OnLevel3BossReached.Broadcast();
		}
	}


	if (Stage != GameStage::Space) {

		// did player reached level 4 speed?
		if (speed >= Level4TriggerSpeed && !Level4Disable /* has developer disabled level4? */) {
			SetStage(GameStage::Space);
			OnLevel4Reached.Broadcast();
		}
	}
}
#pragma optimize("", off)
// Called every frame
void ARaceGameStateBase::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);

	// keep track of delta time history
	if (deltaTimeHistory.Num() != DeltaTimeHistorySize) {
		deltaTimeHistory.Init(0, DeltaTimeHistorySize);
	}

	// reset counter when over limit
	if (fpsIndexCounter >= (DeltaTimeHistorySize-1)) {
		fpsIndexCounter = 0;
	}

	deltaTimeHistory[++fpsIndexCounter] = DeltaTime;

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
			TopSpeed = speed; // theoretical speed
			TopXVelocity = ship->GetSpeed();// physics speed
		}
	}

	MaintainState();

	// clean up invalids utility method
	auto removeInvalids = [](const AActor* a) {
		return a == NULL || !UKismetSystemLibrary::IsValid(a);
	};

	ActiveEnemies.RemoveAll(removeInvalids); // remove invalid enemy actors
	IgnoredByLaserTrace.RemoveAll(removeInvalids); // remove invalid ignored by laser actors
}
#pragma optimize("", on)
void ARaceGameStateBase::AddIgnoredByLaserTrace(AActor* actorToIgnore) {
	IgnoredByLaserTrace.Add(actorToIgnore);	
}

void ARaceGameStateBase::AddActiveEnemy(AActor* enemy) {
	ActiveEnemies.Add(enemy);
}


bool ARaceGameStateBase::RemoveActiveEnemy(AActor* enemy) {
	for (int i = 0; i < ActiveEnemies.Num(); ++i) {
		if (ActiveEnemies[i] == enemy) {
			ActiveEnemies.RemoveAt(i);
			return true;
		}
	}

	return false;
}

void ARaceGameStateBase::ResetXYZGrid(float xOffset) {
	auto w = GetWorld();
	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (pawn) {
		FVector loc = pawn->GetActorLocation();
		FVector newLoc = FVector(loc.X+xOffset, loc.Y, loc.Z);
		w->SetNewWorldOrigin(FIntVector(newLoc.X, newLoc.Y, newLoc.Z)+w->OriginLocation);
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


//////////////////////////////////////////////////////////////////////////
/// Updates/blends camera settings, interpolated during run time.
//////////////////////////////////////////////////////////////////////////
FCameraSettings ARaceGameStateBase::GetCameraSettings(float speed) {

	FCameraSettings settings;

	settings.CameraT = FTransform(
		FRotator(-30, -180, 0),
		FVector(500, 0, 500),
		FVector(1, 1, 1)
	);

	settings.InterpSpeed = 3;
	settings.HudScale = 1;

	settings.BloomIntensity = 0.05;
	settings.BloomThreshold = 0.1;

	// PRELAUNCH
	if (Stage == GameStage::Prelaunch) {

		settings.InterpSpeed = 2;

		settings.BloomIntensity = 0.05;
		settings.BloomThreshold = 0.05;

		settings.HudScale = .9;
		settings.Fov = 120;

		settings.CameraT.SetTranslation(Level1Stage1CameraPosition);
		
		//return settings;

		// DESERT LEVEL 1
	} else if (Stage == GameStage::Desert) {

		settings.InterpSpeed = 3;

		settings.BloomIntensity = 1.5;
		settings.BloomThreshold = 1.5;

		settings.InterpSpeed = 2;
		settings.HudScale = .9;
		settings.Fov = 100;
		settings.CameraT.SetTranslation(Level1Stage1CameraPosition);
		settings.CameraT.SetRotation(Level1Stage1CameraRotation.Quaternion());

		//return settings;

		// DESERT LEVEL 1 BOSS
	} else if (Stage == GameStage::DesertBoss) {

		settings.InterpSpeed = 3;

		settings.BloomIntensity = 1.5;
		settings.BloomThreshold = 1.5;

		settings.InterpSpeed = 1;
		settings.HudScale = 1.2;
		settings.Fov = 120;
		settings.CameraT.SetTranslation(Level1Stage2CameraPosition);
		settings.CameraT.SetRotation(Level1Stage2CameraRotation.Quaternion());

		//return settings;

		// INFINITE HEX (BEGINNING)
	} else if (Stage == GameStage::InfiniteHex) {

		// default setting
		settings.BloomIntensity = 1.5;
		settings.BloomThreshold = 1.5;

		settings.InterpSpeed = .5;
		settings.Fov = 120;
		settings.HudScale = 1.3;
		settings.CameraT.SetTranslation(Level2Stage1CameraPosition);
		settings.CameraT.SetRotation(Level2Stage1CameraRotation.Quaternion());


		if (speed >= Level2ObstacleTriggerspeedThreshold1) {

			settings.CameraT.SetTranslation(Level2Stage2CameraPosition);
			settings.CameraT.SetRotation(Level2Stage2CameraRotation.Quaternion());

		}

		return settings;

	} else if (Stage == GameStage::InfiniteHexBoss) {

		settings.InterpSpeed = 1;

		settings.BloomIntensity = 1.0;
		settings.BloomThreshold = 1.0;

		settings.Fov = 140;
		settings.HudScale = 1.3;
		settings.CameraT.SetTranslation(Level2BossCameraPosition);
		settings.CameraT.SetRotation(Level2BossCameraRotation.Quaternion());

	}
	else if (Stage == GameStage::Labyrinth) {

		settings.InterpSpeed = 3;

		settings.CameraT.SetTranslation(Level3Stage1CameraPosition);
		settings.CameraT.SetRotation(Level3Stage1CameraRotation.Quaternion());
		settings.Fov = 123;

		settings.BloomIntensity = 0.02;
		settings.BloomThreshold = 0.05;

	} else if (Stage == GameStage::LabyrinthBoss) {

		settings.InterpSpeed = 3;

		settings.CameraT.SetTranslation(Level3Stage2CameraPosition);
		settings.CameraT.SetRotation(Level3Stage2CameraRotation.Quaternion());
		settings.Fov = Level3Stage2CameraFOV;

		settings.BloomIntensity = 0.5;
		settings.BloomThreshold = 0.5;

	} else if (Stage == GameStage::Space) {

		settings.InterpSpeed = 3;

		settings.CameraT.SetTranslation(Level4CameraPosition);
		settings.CameraT.SetRotation(Level4CameraRotation.Quaternion());
		settings.Fov = 75;

		settings.BloomIntensity = 0.02;
		settings.BloomThreshold = 0.05;
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

		settings.FogDensity = 0.6;
		settings.FogHeightFaloff = .5;
		settings.FogMaxOpacity = 1;
		//settings.FogStartDist = 1850;
		settings.FogStartDist = 4700;
		settings.FogInscatteringExponent = 10;
		settings.FogInscatteringStartDist = 1900;

		settings.FogInscatteringColor = FLinearColor(.447, .638, 1, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.25, .25, .125, 1);

	} else if(Stage == GameStage::InfiniteHexBoss){

		settings.FogDensity = 1.2;
		settings.FogHeightFaloff = .0001;
		settings.FogMaxOpacity = 1;
		settings.FogStartDist = 1850;
		settings.FogInscatteringExponent = 10;
		settings.FogInscatteringStartDist = 1900;

		settings.FogInscatteringColor = FLinearColor(.447, .638, 1, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(.25, .25, .125, 1);


	} else if (Stage == GameStage::Labyrinth || Stage == GameStage::LabyrinthBoss) {
		
		settings.FogDensity = .19;
		settings.FogHeightFaloff = 0.0001;
		settings.FogMaxOpacity = 1.0;
		settings.FogStartDist = 7000.0;
		settings.FogInscatteringExponent = 1;
		settings.FogInscatteringStartDist = 30000.0;

		settings.FogInscatteringColor = FLinearColor(0.035282, 0.038711, 0.1, 1);
		settings.FogInscatteringColorDirectional = settings.FogInscatteringColor;

	} else if (Stage == GameStage::Space) {

		settings.FogDensity = .0;
		settings.FogHeightFaloff = 0.0001;
		settings.FogMaxOpacity = 1.0;
		settings.FogStartDist = 7000;
		settings.FogInscatteringExponent = 1;
		settings.FogInscatteringStartDist = 30000.0;

		settings.FogInscatteringColor = FLinearColor(0.035282, 0.038711, 0.1, 1);
		settings.FogInscatteringColorDirectional = FLinearColor(0.1664, 0.192091, 0.26, 1);

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
	else if (Stage == GameStage::Labyrinth) {
		ARaceGameStateBase::transitionAtmosphere(ExponentialFog, atmosphereSettings, 4);
	}
	else {
		ARaceGameStateBase::transitionAtmosphere(ExponentialFog, atmosphereSettings, 10);
	}
}

void ARaceGameStateBase::transitionAtmosphere(AExponentialHeightFog* fog, FAtmosphereSettings to, float speed) {

	auto world = GetWorld();

	if (!world) {
		return;
	}

	UExponentialHeightFogComponent* fogC = fog->GetComponent();
	auto delta = world->GetDeltaSeconds();

	fogC->SetFogDensity(FMath::FInterpTo(fogC->FogDensity, to.FogDensity, delta, speed));
	fogC->SetFogHeightFalloff(FMath::FInterpTo(fogC->FogHeightFalloff, to.FogHeightFaloff, delta, speed));
	fogC->SetFogMaxOpacity(FMath::FInterpTo(fogC->FogMaxOpacity, to.FogMaxOpacity, delta, speed));
	fogC->SetStartDistance(FMath::FInterpTo(fogC->StartDistance, to.FogStartDist, delta, speed));
	fogC->SetDirectionalInscatteringExponent(FMath::FInterpTo(fogC->DirectionalInscatteringExponent, to.FogInscatteringExponent, delta, speed));
	fogC->SetDirectionalInscatteringStartDistance(FMath::FInterpTo(fogC->DirectionalInscatteringStartDistance, to.FogInscatteringStartDist, delta, speed));
	fogC->SetFogInscatteringColor(UX::InterpLinearColor(fogC->FogInscatteringColor, to.FogInscatteringColor, delta, speed));
	fogC->SetDirectionalInscatteringColor(UX::InterpLinearColor(fogC->DirectionalInscatteringColor, to.FogInscatteringColorDirectional, delta, speed));

}

// Returns control settings based on Stage of the game and current speed.
FControlSettings ARaceGameStateBase::GetControlSettings(float speed) {

	FControlSettings settings;
	settings.BankingRotationImpulse = 0;
	settings.BankingRotationImpulseMax = 0;
	settings.BankingSpeedImpulse = 0;
	settings.LeapImpulse = 0;
	settings.MaxBankingSpeed = 0;

	if (Stage == GameStage::Prelaunch) {

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

	}
	else if (Stage == GameStage::InfiniteHex || Stage == GameStage::InfiniteHexBoss) {

		settings.BankingRotationImpulse = .3;
		settings.BankingRotationImpulseMax = 20;
		settings.BankingSpeedImpulse = 450;
		settings.LeapImpulse = 8000;
		settings.MaxBankingSpeed = 1500;

		return settings;


	} else if (Stage == GameStage::Labyrinth) {

		settings.BankingRotationImpulse = 1.7;
		settings.BankingRotationImpulseMax = 30;
		settings.BankingSpeedImpulse = 5000;
		settings.LeapImpulse = 35000;
		settings.MaxBankingSpeed = 15000;

	} else if (Stage == GameStage::Space) {

		settings.BankingRotationImpulse = 1.8;
		settings.BankingRotationImpulseMax = 30;
		settings.BankingSpeedImpulse = 7000;
		settings.LeapImpulse = 35000*20;
		settings.MaxBankingSpeed = 60000;

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

	} else if (Stage == GameStage::InfiniteHex || Stage == GameStage::InfiniteHexBoss) {

		settings.Alpha = 1.5;
		settings.Scale = FVector(6.5, 11, 6.5);
		settings.Radius = .5;
		settings.Color = FLinearColor(.289, .375, .362, 1);

		return settings;

	} else if (Stage == GameStage::Labyrinth) {

		 settings.Alpha = 0.0;
		 return settings;

	} else {

		settings.Alpha = 0.0;
		return settings;
	}

	return settings;
}


float ARaceGameStateBase::GetNextStageSpeed() {

	// sanity checks
	bool shipOk = false;
	auto ship = GetRaceShip(shipOk);

	if (!shipOk) {
		return 0;
	}

	// current ship speed
	float currentSpeed = ship->GetTheoreticalSpeed();


	// return first occurrence of speed below any preset threshold
	if ( currentSpeed < Level1BossTriggerSpeed ) {

		return Level1BossTriggerSpeed;

	} else if (currentSpeed < Level2BossTriggerSpeed) {

		return Level2BossTriggerSpeed;

	} else if (currentSpeed < Level3TriggerSpeed) {

		return Level3TriggerSpeed;

	} else if (currentSpeed < Level3Stage2TriggerSpeed) {

		return Level3Stage2TriggerSpeed;

	} else if (currentSpeed < Level4TriggerSpeed) {

		return Level4TriggerSpeed;

	}

	return -1; // suppress compiler
}


void ARaceGameStateBase::GetLaserEffectiveRange(float& effectiveRange, float& falloff) {
	// default
	if (Stage == GameStage::Desert || Stage == GameStage::DesertBoss || Stage == GameStage::Prelaunch) {

		falloff = Level1LaserFalloff;
		effectiveRange = Level1LaserEffectiveRange;
		return;
	
	}else if (Stage == GameStage::InfiniteHex ) {
	
		falloff = Level2LaserFalloff;
		effectiveRange = Level2LaserEffectiveRange;
		return;

	} else if (Stage == GameStage::InfiniteHexBoss) {
	
		falloff = Level2BossLaserFalloff;
		effectiveRange = Level2BossLaserEffectiveRange;
		return;
	
	} else if (Stage == GameStage::Labyrinth || Stage == GameStage::LabyrinthBoss) {

		falloff = Level3LaserFalloff;
		effectiveRange = Level3LaserEffectiveRange;
		return;
	} else if (Stage == GameStage::Space ) {

		falloff = 0;
		effectiveRange = FLT_MAX;
		return;
	}

	falloff = DefaultLaserFalloff;
	effectiveRange = DefaultLaserEffectiveRange;
}


bool ARaceGameStateBase::InRaceCollectCC(int32 amount) {
	CurrentSessionCC += amount;

	OnCCAdded.Broadcast(amount);

	// todo: verification routine here, basic stuff like: is this amount consistent with the current stage? speed? 

	return true;
}

float ARaceGameStateBase::GetStageBoostBlurAmount() {
	// sanity checks
	bool shipOk = false;
	auto ship = GetRaceShip(shipOk);

	if (!shipOk) {
		return 0;
	}

	// current ship speed
	float currentSpeed = ship->GetTheoreticalSpeed();

	if (currentSpeed < 4000) {
		return BoostBlurAmountLevel1;
	} else {
		return BoostBlurAmountLevel2;
	}
}

float ARaceGameStateBase::GetSonicBoomEffectOpacity() {

	float ret = 0.65;

	if (Stage == GameStage::Labyrinth || Stage == GameStage::LabyrinthBoss) {
		ret = Level3SonicBoomMaxOpacity;
	}

	return ret;
}

AActor* ARaceGameStateBase::GetAutoAimTargetSafe(bool& success) {

	if (!LastAutoAimTarget) {
		success = false;
		return NULL;
	}

	if (LastAutoAimTarget->IsValidLowLevel()) {

		auto autoAimTarget = Cast<ARaceActorBase>(LastAutoAimTarget); // ARaceActorBase might have additional data

		if (autoAimTarget) { 
			if (!autoAimTarget->IsAutoAimTarget) { // check to see if this <ARaceActorBase>autoAimTarget is still broadcasting to be targeted (if not it was likely destroyed)
				success = false;
				return NULL;
			}
		}

		success = true;
		return LastAutoAimTarget;
	}

	success = false;
	return NULL;
}

bool ARaceGameStateBase::RequestAttackerStatus(ARaceActorBase* requester) {

	if (!UKismetSystemLibrary::IsValid(CurrentAttacker)) {

		CurrentAttacker = requester;
		return true;
	}

	return false;
}

bool ARaceGameStateBase::ReleaseAttackerStatus(ARaceActorBase* attacker) {
	if (CurrentAttacker == attacker) {
		CurrentAttacker = NULL;
		return true;
	}

	return false;
}

int32 ARaceGameStateBase::GetLevel3Stage1ObstacleCount() {
	if (Stage != GameStage::Labyrinth) {
		return 0;
	}

	// sanity checks
	bool shipOk = false;
	auto ship = GetRaceShip(shipOk);

	if (!shipOk) {
		return 0;
	}

	// current ship speed
	float currentSpeed = ship->GetTheoreticalSpeed();

	if (currentSpeed < 12300) {
		return 8;

	} else if (currentSpeed < 12600) {
		return 10;

	} else if (currentSpeed < 18000) {
		return 11;
	}

	return 0;
}

float ARaceGameStateBase::GetAverageFps() {
	float sum = 0.0;

	for (int32 i = 0; i < deltaTimeHistory.Num(); ++i) {
		sum += deltaTimeHistory[i];
	}

	return 1000.0/((sum / ((float)deltaTimeHistory.Num()))*1000.0);
}