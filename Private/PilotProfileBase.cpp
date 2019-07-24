// Alpha only, not for distribution.


#include "PilotProfileBase.h"

// Sets default values
APilotProfileBase::APilotProfileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APilotProfileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APilotProfileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

