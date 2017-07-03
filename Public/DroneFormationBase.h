// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneFormationBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreeSlotAvailable, USceneComponent*, Marker);


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
	FName gridMarketTagName = "GridMarker";

	// changes data
	int32 previousColumns = 0;
	int32 previousRows = 0;

	void detectAndProcessChanges();
	void realignGrid();
	void drawDebug();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsAutoAimTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector Bounds = FVector(1000,1000,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Columns = 5; // Y
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 Rows = 5; // X

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<USceneComponent*> Positions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<AActor*> Drones;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Events")
	FOnGridUpdate OnGridUpdate;
};