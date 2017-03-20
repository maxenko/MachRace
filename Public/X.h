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
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Y Dist.", Keywords = "Get distance between vectors using Y axis only."), Category = "Machrace|Utility")
	static float GetYDist(FVector a, FVector b) {
		a.X = a.Z = b.X = b.Z = 0;
		return FVector::Dist(a, b);
	}

	static UPrimitiveComponent* GetRootAsPrimitive(AActor* a, bool& success);
	static void DecayRootRotToZero(AActor* a, float delta, float decaySpeed);
	static bool VectorsWithinAngle(FVector a, FVector b, float angle);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Parse DateTime", Keywords = "Parse DateTime in HTTP (RFC 1123) format."), Category = "Machrace|Utility")
	static bool ParseDateTime(const FString & DateTimeString, FDateTime & OutDateTime) {
		return FDateTime::ParseHttpDate(DateTimeString, OutDateTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Total Milliseconds Between", Keywords = "Gets total number of milliseconds between two dates."), Category = "Machrace|Utility")
	static int32 TotalMillisecondsBetween(FDateTime a, FDateTime b) {
		return (int32)(a - b).GetTotalMilliseconds();
	}
};