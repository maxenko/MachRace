// Copyright 2015 - Max Enko

#pragma once
#include "CommonTypes.generated.h"

USTRUCT(BlueprintType)
struct FBufferedSpawnLinearDirective {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	AActor* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	int SpawnsPerTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	TSubclassOf<AActor> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	TArray<FTransform> Transforms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	TArray<AActor*> Ignored;

	FBufferedSpawnLinearDirective() {
		Owner = NULL,
		SpawnsPerTick = 0,
		Type = AActor::StaticClass(),
		Transforms = TArray<FTransform>();
		Ignored = TArray<AActor*>();
	}

	FORCEINLINE bool operator==(const FBufferedSpawnLinearDirective &Other) const
	{
		return
			Owner == Other.Owner
			&& SpawnsPerTick == Other.SpawnsPerTick
			&& Type == Other.Type;

	}
};


USTRUCT(BlueprintType)
struct FTemporaryIndexedRecord {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	int Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	UObject* Parent;

};


USTRUCT(BlueprintType)
struct FComponentDamageRecord {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	USceneComponent* Component;

	FORCEINLINE bool operator==(const FComponentDamageRecord &Other) const {
		return
			Component == Other.Component
			&& Health == Other.Health;
	}
};


USTRUCT(BlueprintType)
struct FControlSettings {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float MaxBankingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float BankingSpeedImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float LeapImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float BankingRotationImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float BankingRotationImpulseMax;
};


UENUM(BlueprintType)
enum class GameStage : uint8 {
	Prelaunch		UMETA(DisplayName = "Prelaunch"),
	Desert 			UMETA(DisplayName = "Plain Desert"),
	DesertBoss 		UMETA(DisplayName = "Plain Desert Boss"),
	InfiniteHex		UMETA(DisplayName = "Infinite Hex"),
	InfiniteHexBoss	UMETA(DisplayName = "Infinite Hex Boss"),
	Labyrinth		UMETA(DisplayName = "Mechanical Labyrinth"),
	LabyrinthBoss	UMETA(DisplayName = "Mechanical Labyrinth Boss"),
	Space			UMETA(DisplayName = "Space"),
};

UENUM(BlueprintType)
enum class DesertTileType : uint8 {
	Empty			UMETA(DisplayName = "Empty"),
	Regular 		UMETA(DisplayName = "Regular"),
	Mines 			UMETA(DisplayName = "Mines"),
};

UENUM(BlueprintType)
enum class Side : uint8 {
	Left			UMETA(DisplayName = "Left"),
	Right 			UMETA(DisplayName = "Right"),
	Center			UMETA(DisplayName = "Center"),
};

USTRUCT(BlueprintType)
struct FCameraSettings {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float Fov;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float BloomIntensity = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float BloomThreshold = 0.2;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float LensFlareIntensity = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float LensFlareThreshold = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float LensFlareBokenSize = 3.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FLinearColor LensFlareColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FTransform CameraT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float HudScale;
};


USTRUCT(BlueprintType)
struct FHexTileDistribuition {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Accelerators = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Decelerators = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Collectables = .005;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ICBM = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Column = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float RotatingColumn = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Press = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Destructible = .01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Standard = .935;
};


USTRUCT(BlueprintType)
struct FAtmosphereSettings {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogHeightFaloff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogMaxOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogStartDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogInscatteringExponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float FogInscatteringStartDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FLinearColor FogInscatteringColorDirectional;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FLinearColor FogInscatteringColor;
};


USTRUCT(BlueprintType)
struct FUnderfadeSettings {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Ship")
	float Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Ship")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Ship")
	FVector Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Ship")
	FLinearColor Color;

};


USTRUCT(BlueprintType)
struct FObjectTransformRecord {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	UObject* Object;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FTransform Transform;

};


UENUM(BlueprintType)
enum class EHexTileChance : uint8
{
	Standard,
	Accelerator,
	Decelerator,
	Column,
	Collectable,
	ICBM,
	RotatingColumn,
	Press
};


USTRUCT(BlueprintType)
struct FFlightNavigationRay {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector To = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Distance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FHitResult Hit;
};

UENUM(BlueprintType)
enum class AutopilotPathStatus : uint8 {
	NoPath			UMETA(DisplayName = "NoPath"),	// completely blocked
	Path	 		UMETA(DisplayName = "Path"),	// obstacles on the way, but need to go around them
	Clear			UMETA(DisplayName = "Clear"),	// no obstacles whatsoever
};

UENUM(BlueprintType)
enum class AutopilotPathCondition : uint8 {
	Blocked			UMETA(DisplayName = "Blocked"),	// completely blocked
	Clear			UMETA(DisplayName = "Clear"),	// no obstacles whatsoever
};

UENUM(BlueprintType)
enum class AutopilotIntent : uint8 {
	DodgeObstacles			UMETA(DisplayName = "DodgeObstacles"),			// completely blocked
	ResumeNonDodgeBehavior	UMETA(DisplayName = "ResumeNonDodgeBehavior"),	// no obstacles whatsoever
};

UENUM(BlueprintType)
enum class MachRaceInputType : uint8 {
	KeyboardInput			UMETA(DisplayName = "KeyboardInput"),	// completely blocked
	JoystickInput			UMETA(DisplayName = "JoystickInput"),	// no obstacles whatsoever
};