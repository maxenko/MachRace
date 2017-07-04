// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneFormationBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReleaseDrone, AActor*, Drone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReassignDrone, AActor*, Drone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreeSlotAvailable, UDroneToFormationLink*, Link);


UCLASS()
class MACHRACE_API UDroneToFormationLink : public UObject {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	AActor* Drone = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|Debug")
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector Bounds = FVector(1000,1000,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Columns = 5; // Y
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Rows = 5; // X

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<USceneComponent*> Positions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MachRace|System")
	TArray<AActor*> Drones;

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
	void LinkDrone(AActor* drone, UDroneToFormationLink* link);
};