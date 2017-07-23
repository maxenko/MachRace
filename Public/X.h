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

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get X Dist.", Keywords = "Get distance between vectors using X axis only."), Category = "Machrace|Utility")
		static float GetXDist(FVector a, FVector b) {
		a.Y = a.Z = b.Y = b.Z = 0;
		return FVector::Dist(a, b);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Z Dist.", Keywords = "Get distance between vectors using Z axis only."), Category = "Machrace|Utility")
		static float GetZDist(FVector a, FVector b) {
		a.X = a.Y = b.X = b.Y = 0;
		return FVector::Dist(a, b);
	}

	static UPrimitiveComponent* GetRootAsPrimitive(AActor* a, bool& success);
	static void DecayRootRotToZero(AActor* a, float delta, float decaySpeed);
	static bool VectorsWithinAngle(FVector a, FVector b, float angle);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Parse DateTime", Keywords = "Parse DateTime in HTTP (RFC 1123) format."), Category = "Machrace|Utility")
	static bool ParseDateTime(const FString & DateTimeString, FDateTime & OutDateTime) {
		return FDateTime::ParseHttpDate(DateTimeString, OutDateTime); // pass-through call, as its not exposed to blueprint
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Total Milliseconds Between", Keywords = "Gets total number of milliseconds between two dates."), Category = "Machrace|Utility")
	static int32 TotalMillisecondsBetween(FDateTime a, FDateTime b) {
		return (int32)(a - b).GetTotalMilliseconds();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Within Cone", Keywords = "Returns true when vector is within given cone of to the camera."), Category = "Machrace|Utility")
	static bool IsWithinCameraView(APlayerCameraManager* cm, FVector to, float within);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Snap to Interval", Keywords = "Snaps to the interval segment. Floored to the lowest end."), Category = "Machrace|Utility")
	static float SnapToInterval(float interval, float v);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Random Vector in Range", Keywords = "Generate random vector in range from min to max."), Category = "Machrace|Utility")
	static FVector RandVecInRange(FVector min, FVector max);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Target Look At Rot", Keywords = "Gets look at target rotation."), Category = "Machrace|Utility")
	static FRotator GetTargetLookAtRot(FTransform actorT, FVector targetLoc);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Y Dist Actors", Keywords = "Gets distance between two actors but only in Y axis."), Category = "Machrace|Utility")
	static float GetYDistBetweenActors(AActor* a, AActor* b);
};