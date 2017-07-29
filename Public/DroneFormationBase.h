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
class MACHRACE_API ADroneFormationBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneFormationBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TArray<FVector> getFormationGrid();
	FName gridMarkerTagName = "GridMarker";

	// changes data
	int32 previousColumns = 0;
	int32 previousRows = 0;

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
	TArray<USceneComponent*> Positions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<ARaceFormationDroneBase*> Drones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<UDroneToFormationLink*> Links;

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
	ARaceFormationDroneBase* GetClosestDroneInAttackPosition(AActor* target, bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assign closest drone.", Keywords = "Assigns closest drone as designated (attack) drone, if there is one."), Category = "MachRace|Gameplay")
	bool AssignClosestDroneIfNoneAreDesignated(AActor* target);
};