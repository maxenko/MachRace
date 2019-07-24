// Alpha only, not for distribution.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PilotProfileBase.generated.h"

UCLASS()
class MACHRACE_API APilotProfileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APilotProfileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
