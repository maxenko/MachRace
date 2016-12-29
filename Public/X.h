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
	static FVector GetRootAngularVelocity(AActor* target);
	static void SetRootLinearVelocity(AActor* target, FVector v, bool addTo = false);
	static void SetRootAngularVelocity(AActor* target, FVector v, bool addTo = false);
	static FVector NullifyY(FVector v);

	static int32 GetYDirMult(FVector a, FVector b) {
		return a.Y < b.Y ? -1 : 1;
	}
	
	static float GetYDist(FVector a, FVector b) {
		a.X = a.Z = b.X = b.Z = 0;
		return FVector::Dist(a, b);
	}
};
