// Alpha only, not for distribution.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "RacePool.generated.h"

UCLASS()
class MACHRACE_API ARacePool : public AActor
{
	GENERATED_BODY()
	
private:

	/* Collection of pools by class type of whats being pooled, 
	   splitting pools into own categories like HexAccelerators, 
	   HexDecelerators etc to reduce lookup times and scans */

	TMap<TSubclassOf<class ARaceActorBase>, TArray<ARaceActorBase*>> pools; 

public:	
	// Sets default values for this actor's properties
	ARacePool();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Return To Pool", Keywords = "Return actor to pool."), Category = "MachRace|Engine")
	int32 ReturnToPool(ARaceActorBase* a, TSubclassOf<class ARaceActorBase> type);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take From Pool", Keywords = "Take actor from pool."), Category = "MachRace|Engine")
	ARaceActorBase* TakeFromPool(TSubclassOf<class ARaceActorBase> type, bool& success, FVector to);

	int32 ClearPool(TSubclassOf<class ARaceActorBase> type);
};
