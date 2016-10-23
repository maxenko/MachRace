// Copyright 2015 - Max Enko

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonTypes.h"
#include "RaceGameStateBase.h"
#include "CustomExtensions.generated.h"



const FString GAMENAME = "machrace_pc";

/**
 * 
 */
UCLASS()
class MACHRACE_API UCustomExtensions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Race State", Keywords = "Gets game specific state."), Category = "MachRace|Utility")
	static ARaceGameStateBase* GetRaceState(UWorld* w);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Dist Sans Z", Keywords = "Distance between points without counting Z component."), Category = "MachRace|Utility|Math")
	static float DistSansZ(FVector from, FVector to);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Motion Blur", Keywords = "Set Motion Blur Camera Player"), Category = Camera)
	static void SetMotionBlur(UCameraComponent* cam, float amount, float max);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Lens Flare Intensity", Keywords = "Set lens flare intensity for the given camera."), Category = Camera)
	static void SetLensFlareIntensity(UCameraComponent* cam, float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "A to B Offset", Keywords = "Get Vector Point Offset Between"), Category = "MachRace|Utility|Math")
	static FVector AtoBOffset(FVector a, FVector b);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Extent To Size", Keywords = "Convert Box Extent To Size"), Category = "MachRace|Utility|Math")
	static FVector ExtentToSize(FVector v);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Remap Value", Keywords = "Remap value from one domain to another."), Category = "MachRace|Utility|Math")
	static float RemapValueToRange(float val, float oldRangeMin, float oldRangeMax, float newRangeMin, float newRangeMax);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Includes Point 2D",  Keywords = "Checks if the point is inside 2D bounds."), Category = "MachRace|Utility|Math")
	static bool OverlapsPoint2D(FVector2D v, float xMin, float xMax, float yMin, float yMax);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Contains point in bounds.", Keywords = "Checks if the point is inside 3D bounds."), Category = "MachRace|Utility|Math")
	static bool OverlapsPoint3D(FVector point, FVector origin, FVector bounds);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Contains point in bounds ignoring Z.", Keywords = "Checks if the point is inside 3D bounds ignoring Z coordinate."), Category = "MachRace|Utility|Math")
	static bool OverlapsPoint3DSansZ(FVector point, FVector origin, FVector bounds);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Filter point set against another.", Keywords = "Filters set points that overloap any points in the second set.."), Category = "MachRace|Utility|Math")
	static TArray<FVector> FilterPointsThatOverlapOtherPoints(TArray<FVector> toFilter, TArray<FVector> toFilterAgainst, float minDist, bool ignoreZ = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Bounds overlap point.", Keywords = "Returns true if any of the bounds overlap the point."), Category = "MachRace|Utility|Math")
	static bool AnyBoundsOverlapPoint(FVector point, TArray<FVector> origins, TArray<FVector> bounds);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Filter points by minimum distance to each other.",  Keywords = "Takes an array of points and returns only the points which are furuther away to each other than the specified distance."), Category = "MachRace|Utility|Math")
	static TArray<FVector> FilterPointsByMinDistance(TArray<FVector> points, float dist);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get points outside given bounds.",  Keywords = "Takes an array of points and returns a filtered array of points that are not overlapping the given bounds."), Category = "MachRace|Utility|Math")
	static TArray<FVector> FilterPointsOverlappingBounds(TArray<FVector> points, FVector boundsOrigin, FVector bounds);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Filter points that fall within any given actors bounds.", Keywords = "Takes an array of points and an array of actors, returns an array of points that are not inside any of the actors bounds."), Category = "MachRace|Utility|Math")
	static TArray<FVector> FilterPointsInsideActorBounds(TArray<FVector> points, TArray<AActor*> actors);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Wrap Index to Array Length", Keywords = "Places index inside length, with overlapping values wrapped at edges."), Category = "MachRace|Utility|Math")
	static int32 WrapIdxTo(int32 Len, int32 index);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get substring by percentage of string.", CompactNodeTitle = "SubstrAlpha", Keywords = "Returns a substring of a string at a given alpha position."), Category = String)
	static FText SubstrAlpha(FString s, int32 len, float alpha, bool& isAtRightEdge);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Slice", CompactNodeTitle = "SLICE", Keywords = "Get a slice of an array."), Category = "Extensions|Array")
	static TArray<FString> ArraySlice(const TArray<FString>& TargetArray, int32 Index, int32 Size);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get random sorted range of floats.", Keywords = "Random range of floats, sorted."), Category = "Math|Array")
	static TArray<float> RandomSortedFloats(int32 Length, float Min, float Max);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Sum array of integers.", Keywords = "Sums a given array of ints."), Category = "Math|Array")
	static int32 SumIntArray(TArray<int32> a);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Random index by weight.", CompactNodeTitle = "RndIdxByWeight", Keywords = "Takes an array of integers representing weights, and picks a random index within the bounds of array based on weight."), Category = "Math|Array")
	static int32 PickRandomIdxByWeight(TArray<int32> weights);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Returns array tail.", Keywords = "Return array tail (everything but the first element)."), Category = "Utility|Array")
	static TArray<int32> Tail(TArray<int32> a);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Url Encode.", Keywords = "Encode text to URL format."), Category = "Utility|Text")
	static FString UrlEncode(const FString url);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Fake Scores", Keywords = "Get fake scores, use for testing only."), Category = "Machrace|Scores")
	static TArray<FString> GetFakeScores(TArray<FString> names, float pct, int32 size);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Spaced Random Vectors", Keywords = "Get random vectors that are spaced from each other."), Category = "Math|Vector")
	static TArray<FVector> GetSpacedRandomVectors(
		UPARAM(DisplayName = "Max Dist Between") float maxDistToEachOther, 
		UPARAM(DisplayName = "Range Min X") float minX,
		UPARAM(DisplayName = "Range Max X") float maxX,
		UPARAM(DisplayName = "Range Min Y") float minY,
		UPARAM(DisplayName = "Range Max Y") float maxY,
		UPARAM(DisplayName = "Range Min Z") float minZ,
		UPARAM(DisplayName = "Range Max Z") float maxZ,
		UPARAM(DisplayName = "Total to generate") int32 count,
		UPARAM(DisplayName = "Limit of tries") int32 maxTries
		);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Percentage based on range and falloff.", Keywords = "Gets percentage distance of one vector to another based on area of effect (range) and falloff."), Category = "Math|Vector")
	static float AffectorWeight(FVector from, FVector to, float range, float faloff);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set camera saturation.", Keywords = "Sets camera saturation in post processing."), Category = Camera)
	static void SetSaturation(UCameraComponent* cam, float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set camera vignette.", Keywords = "Sets camera vignette intensity in post processing."), Category = Camera)
	static void SetVignetteIntensity(UCameraComponent* cam, float amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Level 1 Obstacle count.", Keywords = "Get level 1 obstacle count by speed."), Category = Camera)
	static int32 Level1ObstacleCount(float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Level 1 Cable count.", Keywords = "Get level 1 cable count by speed."), Category = Camera)
	static int32 Level1CableCount(float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "SkewedHex", Keywords = "Generate skewed hex grid coordinates."), Category = "MachRace|Utility|Math")
	static TArray<FVector> GenSkewedHexagonGrid(int32 columns, int32 rows, float size);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "SkewedHexTransforms", Keywords = "Generate skewed hex grid transforms."), Category = "MachRace|Utility|Math")
	static TArray<FTransform> GenSkewedHexagonGridTransforms(int32 columns, int32 rows, float size, float multiplier, FVector2D verticalJitterRange);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "TemplateVs", Keywords = "Generate an array of Transforms from an array of Vectors."), Category = "MachRace|Utility|Math")
	static TArray<FTransform> MakeTemplatedTransforms(TArray<FVector>vectors, FTransform jitter);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Extract Locations", Keywords = "Returns array of Vectors representing locations within given Transform array."), Category = "MachRace|Utility|Math")
	static TArray<FVector> ExtractLocations(TArray<FTransform>transforms);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "RandTWithMax", Keywords = "Generate a random transform from zero to given max."), Category = "MachRace|Utility|Math")
	static FTransform RandomTWithMax(FTransform t);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "SkewedGridDim", Keywords = "Calculate skewed grid size / dimensions."), Category = "MachRace|Utility|Math")
	static FVector2D GetSkewedHexGridDimensions(int32 cols, int32 rows, float cellSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "IsInRange", Keywords = "Does value fall in range or not."), Category = "MachRace|Utility|Math")
	static bool IsInRange(float Value, float Min, float Max);

	static FLinearColor InterpLinearColor(FLinearColor current, FLinearColor to, float delta, float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GameStage Name", Keywords = "Get GameStage name from GameStage enum."), Category = "MachRace|Utility|Text")
	static FString GetStageName(GameStage s);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Is Ship", Keywords = "True if actor is a child of RaceShipBase."), Category = "MachRace|Utility")
	static bool IsShip(AActor* actor);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Zero World To", Keywords = "Zeroes world origin to specified actor world position."), Category = "MachRace|Utility")
	static void ZeroWorldTo(UWorld* w, FVector newOrigin);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Rand Points", Keywords = "Generates random vectors within given box."), Category = "MachRace|Utility")
	static TArray<FVector> GenerateRandomPointsWithinBounds(
		FVector bounds,
		int32 numberOfRandomPoints,
		FVector origin = FVector(0,0,0),
		FVector2D XClamp = FVector2D(-999999, 999999),
		FVector2D YClamp = FVector2D(-999999, 999999),
		FVector2D ZClamp = FVector2D(-999999, 999999)
	);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Trace Single", Keywords = "Traces single trace by channel.", AutoCreateRefTerm = "ignored"), Category = "MachRace|Utility")
	static FHitResult TraceSingle(UWorld* w, FVector from, FVector to, TArray<AActor*> ignored);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Weighted Chance", Keywords = "Returns true or false based on chance within 0-1 range."), Category = "MachRace|Utility")
		static bool WeightedChance(float chance) {
		return( FMath::RandRange(0.0f,1.0f) < FMath::Clamp(chance,0.0f,1.0f) ) ? false : true;
	}

};