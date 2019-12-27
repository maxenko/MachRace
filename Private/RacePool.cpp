#include "RacePool.h"

// Sets default values
ARacePool::ARacePool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARacePool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARacePool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma optimize("", off)
int32 ARacePool::ReturnToPool(ARaceActorBase* a, TSubclassOf<class ARaceActorBase> type) {

	if (!pools.Contains(type)) {
		TArray<ARaceActorBase*> pool;
		pools.Add(type,pool);
	}

	TArray<ARaceActorBase*> pool = pools.FindOrAdd(type);

	if (!IsValid(a)) {
		return pool.Num();
	}

	if (!pool.Contains(a)) { // don't insert actor into pool if its already there
		pool.Add(a);
		a->SetActorLocation(a->PoolHideLocation);
		a->Pooled = true;
		pools.Emplace(type,pool);
	}

	return pool.Num();
}


ARaceActorBase* ARacePool::TakeFromPool(TSubclassOf<class ARaceActorBase> type, bool &success, FVector to) {

	TArray<ARaceActorBase*> pool = pools.FindOrAdd(type);

	if (pool.Num() > 0) {

		ARaceActorBase* unpooled = pool.GetData()[0];
		pool.RemoveAt(0,1,true);

		pools.Emplace(type,pool);

		success = true;

		unpooled->Pooled = false;
		unpooled->SetActorLocation(to);
		unpooled->ReceivePoolBeginPlay();
		return unpooled;
	}
	
	success = false;
	return NULL;
}
#pragma optimize("", on)

int32 ARacePool::ClearPool(TSubclassOf<class ARaceActorBase> type) {
	/*
	auto pool = pools.Find(type);

	if (!pool) {
		return pools.Num();
	}

	for (ARaceActorBase* actor : *pool) {
		actor->Pooled = false;
		actor->ReceiveDestroyedByPoolClear();
		actor->Destroy();
	}

	pools.Remove(type);
	*/
	return pools.Num();
}