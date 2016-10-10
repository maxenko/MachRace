// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "CommonTypes.h"
#include "RaceContentInstancer.generated.h"


UCLASS()
class MACHRACE_API ARaceContentInstancer : public AActor {
	GENERATED_BODY()

private:
	TArray<UInstancedStaticMeshComponent*> dictionary;
	TArray<FBufferedSpawnLinearDirective> bufferedDirectives;

public:	
	// Sets default values for this actor's properties
	ARaceContentInstancer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstStandardHex;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Instance", Keywords = "Create instance of static mesh, in either new or existing ISM component."), Category = "MachRace|Engine")
	void CreateInstance(UStaticMesh* mesh, FTransform t);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Instance", Keywords = "Create instance of static mesh, in either new or existing ISM component."), Category = "MachRace|Engine")
	void CreateInstanceLocally(UStaticMesh* mesh, FTransform t);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Initiate Buffered Spawn", Keywords = "Initiates buffered spawn, of X entities per frame. Runs in background."), Category = "MachRace|Engine")
	void SpawnBuffered(FBufferedSpawnLinearDirective directive);

};
