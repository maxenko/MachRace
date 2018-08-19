// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "RaceFormationDroneBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDesignated, bool, Designated);


UCLASS()
class MACHRACE_API ARaceFormationDroneBase : public ARaceActorBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaceFormationDroneBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool abandoning = false;

	FTimerHandle wobbleTimer;
	FTimerHandle scanForTargetTimer;
	FTimerHandle abandonTimer;
	FVector wobbleOffset = FVector::ZeroVector;
	void generateRandomOffset();
	bool previousTargetStatus = false;
	bool previousDesignated = false;

	void triggerAbandon() {	abandoning = true; }

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AssignPosition(USceneComponent* position);
	void moveTo(FVector to, float delta, FVector speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool DrawDebug = false;

	//////////////////////////////////////////////////////////////////////////
	// System
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector AbandonOffset = FVector(0, 0, 10000);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector FollowSpeed = FVector(10, 10, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector AbandonSpeed = FVector(10, 10, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Designatable = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	bool FrontDrone = false;

	/** Wether or not this drone is currently designated to shoot the player.
	When Drone loses target, this property gets reset to false. Drone formation will
	automatically assign this property when there are not designated drones, for example
	it will assign the closest drone to player.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool DesignatedDrone = false;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Abandon Formation", Keywords = "Mark drone as having abandoned the formation."), Category = "MachRace|Gameplay")
	void AbandonFormation(float delay);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Target Within Cone", Keywords = "Mark drone as having abandoned the formation."), Category = "MachRace|Gameplay")
	bool IsTargetWithinCone();


	//////////////////////////////////////////////////////////////////////////
	// Gameplay
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Wobble = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector2D WobbleRandomRange = FVector2D(.1, .5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector WobbleOffsetMin = FVector(-2, -2, -2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector WobbleOffsetMax = FVector(2, 2, 2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool ScanEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ScanInterval = .3;

	/* Designated if the drone has been marked as dead. Which might precede DestroyActor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Dead = false;

	/** Controls when target can be fired on, based on each axis individual distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector FireTriggerDistances = FVector(3000, 150, 150);

	//////////////////////////////////////////////////////////////////////////
	// Events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnDesignated OnDesignated;

};
