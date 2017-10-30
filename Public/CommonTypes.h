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
	CitySubmerged	UMETA(DisplayName = "City Submerged"),
};


USTRUCT(BlueprintType)
struct FCameraSettings {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	float Fov;

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
	float Standard = .955;
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
	ICBM
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
