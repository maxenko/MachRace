// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Pawn.h"
#include "RaceGameStateBase.h"
#include "CommonTypes.h"
#include "RaceActorBase.generated.h"

UCLASS()
class MACHRACE_API ARaceActorBase : public AActor /*APawn*/ {
	GENERATED_BODY()

private:

	TArray<FObjectTransformRecord> transformRecord;
	
public:	
	// Sets default values for this actor's properties
	ARaceActorBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Ignored Actors", Keywords = "Optional utility collection of actors to ignore, if this actor needs to."), Category = "MachRace|System")
	TArray<AActor*> IgnoredActors;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	ARaceGameStateBase* State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsAutoAimTarget = false;

	ARaceGameStateBase* GetState();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remember Transform", Keywords = "Records transform of given object at the time of calling this function."), Category = "MachRace|Engine")
	void RememberTransform(UObject* obj, FTransform transform);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Recall Transform", Keywords = "Recalls transform of the object."), Category = "MachRace|Engine")
	bool RecallTransform(UObject* obj, FTransform& transform);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Instancer", Keywords = "Gets MachRace instancer, same as Sate->GetInstancer."), Category = "MachRace|Utility")
	ARaceContentInstancer* GetInstancer(bool& instancer);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Destroy Children", Keywords = "Destroys any actors in Children array."), Category = "MachRace|Engine")
	int32 DestroyChildren(bool buffered = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Child", Keywords = "Adds to internal Children array, which can be cleaned up by calling Destroy Children"), Category = "MachRace|Engine")
	void AddChild(AActor* a);
	
};
