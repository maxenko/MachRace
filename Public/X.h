// Copyright 2015 - Max Enko

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "X.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API UX : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static FVector GetRootLinearVelocity(AActor* target);
	static void SetRootLinearVelocity(AActor* target, FVector v, bool addTo = false);
};
