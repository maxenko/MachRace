// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceFormationDroneBase.h"
#include "DroneFormationBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReleaseDrone, ARaceFormationDroneBase*, Drone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreeSlotAvailable, UDroneToFormationLink*, Link);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReassignDrone, ARaceFormationDroneBase*, Drone, USceneComponent*, position);


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
class MACHRACE_API ADroneFormationBase : public AActor {
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

	TArray<int32> ColumnSizes;

	void detectAndProcessChanges();
	void realignGrid();
	void drawDebug();
	void relinkDrones();
	bool isThereADesignatedDrone();
	void cleanDestroyedDrones();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool DrawDebug = false;

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
	int32 Columns = 5; // Y
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Rows = 5; // X

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	int32 Count = 0; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<USceneComponent*> Positions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<FDroneFormationSquareIndex> Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<ARaceFormationDroneBase*> Drones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<UDroneToFormationLink*> Links;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<int32> ColumnCounts;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnGridUpdate OnGridUpdate;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnReleaseDrone OnReleaseDrone;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnReassignDrone OnReassignDrone;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnFreeSlotAvailable OnFreeSlotAvailable;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Link Drone", Keywords = "Links a given actor as a drone to the formation."), Category = "MachRace|Gameplay")
	void LinkDrone(ARaceFormationDroneBase* drone, UDroneToFormationLink* link);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Attack Ready Drone", Keywords = "Gets drone that is in alignment to attack."), Category = "MachRace|Gameplay")
	ARaceFormationDroneBase* GetClosestDroneInAttackPosition(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assign closest drone.", Keywords = "Assigns closest drone as designated (attack) drone, if there is one."), Category = "MachRace|Gameplay")
	bool AssignClosestDroneIfNoneAreDesignated();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Formation Count", Keywords = "Get total count of slots in the formation."), Category = "MachRace|Gameplay")
	int32 GetFormationCount() {
		return Rows * Columns;
	}
};