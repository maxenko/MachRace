// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "RaceFormationDroneBase.h"
#include "DroneFormationBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReleaseDrone, ARaceFormationDroneBase*, Drone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFreeSlotAvailable, UDroneToFormationLink*, Link, int32, column, int32, row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReassignDrone, ARaceFormationDroneBase*, Drone, USceneComponent*, position);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDroneDestroyed, USceneComponent*, Marker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllDronesDestroyed);


USTRUCT(BlueprintType)
struct FDroneFormationSquareIndex {
	GENERATED_USTRUCT_BODY()

		FDroneFormationSquareIndex(FVector v = FVector::ZeroVector, int32 col = 0, int32 row = 0) : Vector(v), Row(row), Column(col) {}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Logic")
	FVector Vector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Logic")
	int32 Row = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Logic")
	int32 Column = 0;

	/** Position that a drone attempts to stay at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Logic")
	USceneComponent* Marker = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Logic")
	ARaceFormationDroneBase* Drone = NULL;
};


UCLASS()
class MACHRACE_API UDroneToFormationLink : public UObject {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	ARaceFormationDroneBase* Drone = NULL;

	/** Designated position in formation, a drone will attempt to follow.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	USceneComponent* Position;
};


UCLASS()
class MACHRACE_API ADroneFormationBase : public ARaceActorBase {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneFormationBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TArray<FDroneFormationSquareIndex> getFormationGrid();
	FName gridMarkerTagName = "GridMarker"; // used to tag grid marker scene components, so its easy to identify them

	// changes data
	int32 previousColumns = 0;
	int32 previousRows = 0;

	TArray<UDroneToFormationLink*> toBeSpawned;

	void detectAndProcessChanges();
	void realignGrid();
	void drawDebug();
	void relinkDrones();
	bool isThereADesignatedDrone();
	void cleanDestroyedDrones();
	int32 findLargestColumnSize();
	FTimerHandle spawnTimer;
	void updateColumnCounts();
	void broadcastDroneSpawn();
	int32 getEmptySlotsCount(); // figures out how many drones are missing from full formation count
	ARaceFormationDroneBase* findFrontFacingDrone(int32 colIdx);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool ShowDebug = false;

	/** Target to follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector Bounds = FVector(1000,1000,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float AttackTriggerMinYDist = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool EnableSpawns = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Columns = 0; // Y
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Rows = 0; // X

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	int32 Count = 0; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<USceneComponent*> Positions;

	/* Grid index, used by game logic for drone positions / indexes within grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|Gameplay")
	TArray<FDroneFormationSquareIndex> Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<ARaceFormationDroneBase*> Drones;

	/* Actual links that link drones to the formation. Used for formation decisions when drones are abandoned or reassigned to new formation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<UDroneToFormationLink*> Links;

	/** Keep count of how many drones are in each column. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<int32> ColumnCounts;

	/* Delay between drone spawns when empty slots are available. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DroneSpawnFrequence = .2;

	//////////////////////////////////////////////////////////////////////////
	// Events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnGridUpdate OnGridUpdate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnReleaseDrone OnReleaseDrone;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnReassignDrone OnReassignDrone;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnFreeSlotAvailable OnFreeSlotAvailable;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnDroneDestroyed OnDroneDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnAllDronesDestroyed OnAllDronesDestroyed;


	//////////////////////////////////////////////////////////////////////////
	// Utility and other functions
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Link Drone", Keywords = "Links a given actor as a drone to the formation."), Category = "MachRace|System")
	void LinkDrone(ARaceFormationDroneBase* drone, UDroneToFormationLink* link);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Attack Ready Drone", Keywords = "Gets drone that is in alignment to attack."), Category = "MachRace|System")
	ARaceFormationDroneBase* GetClosestDroneInAttackPosition(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assign closest drone.", Keywords = "Assigns closest drone as designated (attack) drone, if there is one."), Category = "MachRace|System")
	bool AssignClosestDroneIfNoneAreDesignated();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Formation Count", Keywords = "Get total count of slots in the formation."), Category = "MachRace|System")
	int32 GetFormationCount() {
		return Rows * Columns;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get open slot count", Keywords = "Get total number of slots in the formation that do not have drones assigned."), Category = "MachRace|System")
	int32 GetOpenSlotCount() {
		return this->getEmptySlotsCount();
	}

	/** This attempts to find an offset so the drone formation is position in front of player, with the maximum amount of drones in a column. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find offset.", Keywords = "Finds an offset (from the center of formation) to the random longest column of drones."), Category = "MachRace|Gameplay")
	float FindLogicalFormationOffset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Random designatable drone.", Keywords = "Picks a random drone at the tail of one of the columns, in columns that have drones."), Category = "MachRace|Gameplay")
	ARaceFormationDroneBase* PickRandomDroneToDesignateV2(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Rediscover Front Facing drones", Keywords = "Find and mark front facing drones in each column."), Category = "MachRace|System")
	void RediscoverFrontFacingDrones();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset Spawn Timer", Keywords = "Resets spawn drone timer, must be called manually after updating DroneSpawnFrequence to reflect changes."), Category = "MachRace|Gameplay")
	void ResetTimer();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find Drone Index", Keywords = "Scans the Index of drones and returns an index that matches the given drone."), Category = "MachRace|System")
	FDroneFormationSquareIndex FindDroneIndex(ARaceFormationDroneBase* drone, bool& success);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Is Column Empty?", Keywords = "Scans the Index and returns true if given column is empty."), Category = "MachRace|System")
	bool IsColumnEmpty(int columnIndex);
};