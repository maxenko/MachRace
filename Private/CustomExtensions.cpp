// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "CustomExtensions.h"
#include "GenericPlatformHttp.h"
#include "RaceShipBase.h"

#include "Kismet/KismetMathLibrary.h"


#define LOCTEXT_NAMESPACE "MachRace" 


ARaceGameStateBase* UCustomExtensions::GetRaceState(UWorld* w) {
	if (!w) {
		return NULL;
	}

	auto _state = w->GetGameState();
	ARaceGameStateBase* state = Cast<ARaceGameStateBase>(_state);
	return state;
}

float UCustomExtensions::DistSansZ(FVector from, FVector to) {
	return FVector::Dist(FVector(from.X, from.Y, 0), FVector(to.X, to.Y, 0));

}

TArray<FVector> UCustomExtensions::GenerateRandomPointsWithinBounds(
	FVector bounds,
	int32 numberOfRandomPoints,
	FVector origin,
	FVector2D XClamp,
	FVector2D YClamp,
	FVector2D ZClamp
) {
	TArray<FVector> points;

	for (int32 i = 0; i < numberOfRandomPoints; ++i) {

		float x_rand = FMath::FRandRange(-bounds.X, bounds.X);
		float y_rand = FMath::FRandRange(-bounds.Y, bounds.Y);
		float z_rand = FMath::FRandRange(-bounds.Z, bounds.Z);

		if (x_rand < XClamp.X || x_rand > XClamp.Y) {
			x_rand = FMath::GetMappedRangeValueClamped(FVector2D(-bounds.X, bounds.X), XClamp, x_rand);
		}

		if (y_rand < YClamp.X || y_rand > YClamp.Y) {
			y_rand = FMath::GetMappedRangeValueClamped(FVector2D(-bounds.Y, bounds.Y), YClamp, y_rand);
		}

		if (z_rand < ZClamp.X || z_rand > ZClamp.Y) {
			z_rand = FMath::GetMappedRangeValueClamped(FVector2D(-bounds.Z, bounds.Z), ZClamp, z_rand);
		}

		points.Add(FVector(x_rand, y_rand, z_rand)+origin);
	}

	return points;
}


void UCustomExtensions::SetMotionBlur(UCameraComponent* cam, float amount, float max){
	cam->PostProcessSettings.MotionBlurAmount = amount;
	cam->PostProcessSettings.MotionBlurMax = max;
}

void UCustomExtensions::SetSaturation(UCameraComponent* cam, float amount) {
	cam->PostProcessSettings.FilmSaturation = amount;
}

void UCustomExtensions::SetVignetteIntensity(UCameraComponent* cam, float amount) {
	cam->PostProcessSettings.VignetteIntensity = amount;
}

void UCustomExtensions::SetLensFlareIntensity(UCameraComponent* cam, float amount){
	auto intensity = amount < 0.0 ? 0.0 : amount;
	cam->PostProcessSettings.LensFlareIntensity = intensity;
}

FVector UCustomExtensions::AtoBOffset(FVector a, FVector b){
	auto diff = a - b;

	return FVector(
		FMath::Abs(diff.X),
		FMath::Abs(diff.Y),
		FMath::Abs(diff.Z)
		);
}

FVector UCustomExtensions::ExtentToSize(FVector v){

	auto absv = v.GetAbs()*2;

	return FVector(
		absv.X, absv.Y, absv.Z
		);
}

float UCustomExtensions::RemapValueToRange(float val, float oldRangeMin, float oldRangeMax, float newRangeMin, float newRangeMax ){
	return (((val - oldRangeMin) * (newRangeMax - newRangeMin)) / (oldRangeMax - oldRangeMin)) + newRangeMin;
}

bool UCustomExtensions::OverlapsPoint2D(FVector2D v, float xMin, float xMax, float yMin, float yMax){
	return v.X >= xMin && v.X <= xMax && v.Y >= yMin && v.Y >= yMax;
}

bool UCustomExtensions::OverlapsPoint3D(FVector point, FVector origin, FVector bounds){

	auto xMin = origin.X - bounds.X;
	auto xMax = origin.X + bounds.X;
	auto yMin = origin.Y - bounds.Y;
	auto yMax = origin.Y + bounds.Y;
	auto zMin = origin.Z - bounds.Z;
	auto zMax = origin.Z + bounds.Z;

	return
		point.X >= xMin && point.X <= xMax &&
		point.Y >= yMin && point.Y <= yMax &&
		point.Z >= zMin && point.Z <= zMax;
}

bool UCustomExtensions::OverlapsPoint3DSansZ(FVector point, FVector origin, FVector bounds){

	float xMin = origin.X - bounds.X;
	float xMax = origin.X + bounds.X;
	float yMin = origin.Y - bounds.Y;
	float yMax = origin.Y + bounds.Y;

	return
		point.X >= xMin && point.X <= xMax &&
		point.Y >= yMin && point.Y <= yMax;
}

TArray<FVector> UCustomExtensions::FilterPointsThatOverlapOtherPoints(TArray<FVector> toFilter, TArray<FVector> toFilterAgainst, float minDist, bool ignoreZ) {
	TArray<FVector> filtered;

	auto isTooColose = [&](FVector point) {
		for (auto tfa : toFilterAgainst) {
			if (!ignoreZ) {
				if (FVector::Dist(point, tfa) < minDist) {
					return true;
				}
			} else {
				if (UCustomExtensions::DistSansZ(point, tfa) < minDist) {
					return true;
				}
			}
		}

		return false;
	};

	for (auto tf : toFilter) {
		if (!isTooColose(tf)) {
			filtered.Add(tf);
		}
	}

	return filtered;
}

bool UCustomExtensions::AnyBoundsOverlapPoint(FVector point, TArray<FVector> origins, TArray<FVector> bounds){

	for (int32 i = 0; i < origins.Num() && i < bounds.Num(); ++i){
		if ( UCustomExtensions::OverlapsPoint3D(point, origins[i], bounds[i])){
			return true;
		}
	}

	return false;
}

TArray<FVector> UCustomExtensions::FilterPointsByMinDistance(TArray<FVector> points, float dist){

	auto filtered = TArray < FVector >();
	auto tooClose = TArray < FVector >();

	for (const FVector& pOuter : points){
		for (const FVector& pInner : points){
			if ( !FVector::PointsAreSame(pInner,pOuter) && FVector::PointsAreNear(pInner,pOuter,dist) ){
				tooClose.Add(pInner);
			}
		}
	}

	for (const FVector& p : points){
		if (!tooClose.Contains(p)){
			filtered.Add(p);
		}
	}

	return filtered;
}

TArray<FVector> UCustomExtensions::FilterPointsOverlappingBounds(TArray<FVector> points, FVector boundsOrigin, FVector bounds){

	auto nonOverlappingPoints = TArray < FVector >();

	for (const FVector& p : points){
		if (!UCustomExtensions::OverlapsPoint3D(p, boundsOrigin, bounds))	{
			nonOverlappingPoints.Add(p);
		}
	}

	return nonOverlappingPoints;
}

TArray<FVector>  UCustomExtensions::FilterPointsInsideActorBounds(TArray<FVector> points, TArray<AActor*> actors){
	
	auto filteredPoints = TArray < FVector >(points);

	for (const AActor* a : actors){
		FVector o;
		FVector e;
		a->GetActorBounds(false,o,e);
		filteredPoints = UCustomExtensions::FilterPointsOverlappingBounds(filteredPoints, o, e);
	}

	return filteredPoints;
	
}

int32 UCustomExtensions::WrapIdxTo(int32 Len, int32 index){

	if (index == 0){
		return index;
	}

	return ((index*(index - Len - (index % Len)) / abs(index) + index + Len + (index % Len)) >> 1) % Len;

}

FText UCustomExtensions::SubstrAlpha(FString s, int32 len, float alpha, bool& atEnd) {

	alpha = FMath::Clamp((float) alpha, (float) 0, (float) 1.0);
	len = FMath::Clamp((int32) len, (int32) 0, (int32) s.Len());

	int32 alphaPos = alpha * s.Len();
	atEnd = (alphaPos + len) >= s.Len();

	FString segment;
	if (atEnd) {
		segment = s.Mid(s.Len() - len, len);
	} else {
		segment = s.Mid(alphaPos, len);
	}

	return FText::FromString(segment);

}

TArray<FString> UCustomExtensions::ArraySlice(const TArray<FString>& TargetArray, int32 Index, int32 Size){
	
	TArray<FString> slice;
	auto to = Index + Size;

	if (Size == 0 || to > TargetArray.Num()){
		slice.Empty();
		return slice;
	}

	Index = FMath::Clamp(Index, 0, TargetArray.Num());
	Size = FMath::Clamp(Size, 0, TargetArray.Num() - 1);

	for (auto i = Index; i < to; ++i){
		slice.Add(TargetArray[i]);
	}

	return slice;
}

TArray<float> UCustomExtensions::RandomSortedFloats(int32 Length, float Min, float Max){

	TArray<float> list;

	for (auto i = 0; i < Length; ++i){
		list.Add(FMath::FRandRange(Min, Max));
	}
	
	list.Sort();
	return list;
}

int32 UCustomExtensions::SumIntArray(TArray<int32> a){
	int32 sum = 0;
	for (int32 i : a){
		sum += i;
	}
	return sum;
}

TArray<int32> UCustomExtensions::Tail(TArray<int32> a){

	TArray<int32> tail;

	// if array length is 1 or 0, return empty tail
	if (a.Num() <= 1){
		return tail;
	}

	for (auto i = 1; i < a.Num(); ++i){
		tail.Add(a[i]);
	}

	return tail;
}

int32 UCustomExtensions::PickRandomIdxByWeight(TArray<int32> weights){

	if (weights.Num() == 0){
		return 0;
	}

	TArray<int32> unsorted;
	unsorted.Append(weights); // keep unsorted version
	weights.Sort(); // we sort values to match sequentially below
	auto sum = UCustomExtensions::SumIntArray(weights);
	auto rndWeight = FMath::RandRange(0, sum);

	for (auto i = 0; i < weights.Num(); ++i){
		if (rndWeight < weights[i]){
			return unsorted.Find(weights[i]); // return unsorted index
		}
	}

	return 0; // should never reach this!
}

FString UCustomExtensions::UrlEncode(const FString url) {
	return FGenericPlatformHttp::UrlEncode(url);
}

TArray<FString> UCustomExtensions::GetFakeScores(TArray<FString> names, float pct, int32 size){

	TArray<FString> scores;
	auto ctr = 0;
	auto maxspeed = 27312;
	pct = FMath::Clamp((float)pct, (float)0, (float) 100.0);

	for (FString name : names) {
		
		FString rank = FString::Printf(TEXT("%d"), ++ctr);
		maxspeed -= 17;
		auto mph = FString::Printf(TEXT("%d"), maxspeed );

		FString formatted = FText::Format(LOCTEXT("ScoreFormat", "{0};{1};{2}"), FText::FromString(rank), FText::FromString(mph), FText::FromString(name)).ToString();

		scores.Add(formatted);
	
	}

	int32 pageStart = FMath::TruncToInt( UCustomExtensions::RemapValueToRange( pct, 0.0, 100.0, 0.0, (float)scores.Num() ) );

	if (pageStart + size > scores.Num()) {
		return UCustomExtensions::ArraySlice(scores, scores.Num()-size, size);
	}
	else {
		return UCustomExtensions::ArraySlice(scores, pageStart, size);
	}

}

TArray<FVector> UCustomExtensions::GetSpacedRandomVectors(float maxDistToEachOther, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, int32 count, int32 maxTries) {

	TArray<FVector> points;

	// checks if the vector is too close to any others.
	auto tooClose = [&points, maxDistToEachOther](FVector v) {
		for (auto i = 0; i < points.Num(); ++i) {
			auto dist = FVector::Dist(points[i], v);
			if (dist < maxDistToEachOther) {
				return true;
			}
		}
		return false;
	};

	for (auto i = 0; i < maxTries; ++i) {

		auto v = FVector(
			FMath::RandRange(minX, maxX),
			FMath::RandRange(minY, maxY),
			FMath::RandRange(minZ, maxZ)
			);

		if (!tooClose(v)) {
			points.Add(v);
		}

		if (points.Num() >= count) {
			break;
		}
	}

	return points;
}

float UCustomExtensions::AffectorWeight(FVector from, FVector to, float range, float faloff) {

	auto dist = FVector::Dist(from, to);

	if (dist > range) {

		return 0.0;

	} else if( dist < faloff ) {

		return  1.0;

	} else {

		auto rangeFrom = FVector2D(range, faloff);
		auto rangeTo = FVector2D(0.0, 1.0);
		return FMath::GetMappedRangeValueClamped(rangeFrom, rangeTo, dist);

	}
}

int32 UCustomExtensions::Level1ObstacleCount(float speed) {
	if (speed < 760) {
		return 0;
	} else if (speed < 900) {
		return 10;
	} else if (speed < 1000) {
		return 10;
	} else if (speed < 1100) {
		return 10;
	} else if (speed < 1200) {
		return 9;
	} else if (speed < 1400) {
		return 8;
	} else if (speed < 2000) {
		return 5;
	} else if (speed < 2600) {
		return 4;
	}

	return 0;
}

int32 UCustomExtensions::Level1CableCount(float speed) {

	if (speed < 2600) {
		return FMath::RandRange(2, 5);;
	} else {
		return 0;
		//return FMath::RandRange(0, 0);
	}

	return 0;
}

TArray<FVector> UCustomExtensions::GenSkewedHexagonGrid(int32 columns, int32 rows, float size) {

	// flat top orientation

	TArray<FVector> coords;
	float height	= size * 2;
	float width		= FMath::Sqrt(3) / 2 * height;
	float hOffset	= width / 2;

	auto vOffset = [height](int row) { return row == 0 ? 0 : height*.25; }; // 1st row is 1/3 of the offset

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < columns; ++col) {

			float x = col * width + hOffset;
			float y = height / 2 + vOffset(row) * row;

			coords.Add( FVector(x,y,0) );
		}

		hOffset += width / 2;
	}

	return coords;
}

TArray<FTransform> UCustomExtensions::GenSkewedHexagonGridTransforms( int32 columns, int32 rows, float size, float multiplier, FVector2D verticalJitterRange ) {
	auto vs = UCustomExtensions::GenSkewedHexagonGrid(columns, rows, size);

	TArray<FTransform> ts;
	
	for (auto v : vs) {
		auto t = FTransform();
		v.Z += FMath::RandRange(verticalJitterRange.X, verticalJitterRange.Y); // apply vertical jitter
		t.SetTranslation(v*multiplier);
		t.SetScale3D(FVector(1, 1, 1)*multiplier);
		ts.Add(t);
	}

	return ts;
}

TArray<FTransform> UCustomExtensions::MakeTemplatedTransforms(TArray<FVector>vectors, FTransform t) {
	TArray<FTransform> ret;

	for (auto v : vectors) {
		ret.Add(
			FTransform(t.GetRotation(), v, t.GetScale3D())
		);
	}

	return ret;
}

TArray<FVector> UCustomExtensions::ExtractLocations(TArray<FTransform>transforms) {

	TArray<FVector> vs;
	for (FTransform t : transforms) {
		vs.Add(t.GetLocation());
	}

	return vs;
}

FTransform UCustomExtensions::RandomTWithMax(FTransform t) {
	return FTransform (


	);
}

FVector2D UCustomExtensions::GetSkewedHexGridDimensions(int32 cols, int32 rows, float cellSize) {

	float height = cellSize * 2;
	float width = FMath::Sqrt(3) / 2 * height;
	float oddEvenOffset = (FMath::Sqrt(3) / 2 * height) / 4;
	float horisontalOffset = (FMath::Sqrt(3) / 2 * height) * (FMath::Sqrt(2.5) / 4);

	if (cols % 2 == 0) {
		oddEvenOffset = -oddEvenOffset;
		horisontalOffset = -width/8;
	}

	return FVector2D(
		rows * height / 2 + width / 2,
		cols * width / 2 + horisontalOffset
	);

}

bool UCustomExtensions::IsInRange(float Value, float Min, float Max){
	return ((true ? (Value >= Min) : (Value > Min)) && (true ? (Value <= Max) : (Value < Max)));
}


FLinearColor UCustomExtensions::InterpLinearColor(FLinearColor current, FLinearColor to, float delta, float speed) {
	FLinearColor color;

	color.R = UKismetMathLibrary::FInterpTo(current.R, to.R, delta, speed);
	color.G = UKismetMathLibrary::FInterpTo(current.G, to.G, delta, speed);
	color.B = UKismetMathLibrary::FInterpTo(current.B, to.B, delta, speed);
	color.A = UKismetMathLibrary::FInterpTo(current.A, to.A, delta, speed);

	return color;
}

FString UCustomExtensions::GetStageName(GameStage s) {
	if (s == GameStage::Desert) {
		return "Desert";
	} else if (s == GameStage::DesertBoss) {
		return "DesertBoss";
	} else if (s == GameStage::InfiniteHex) {
		return "InfiniteHex";
	} else if (s == GameStage::Prelaunch) {
		return "Prelaunch";
	}

	return "Unknown";
}

bool UCustomExtensions::IsShip(AActor* actor) {
	return actor->IsA(ARaceShipBase::StaticClass());
}

void UCustomExtensions::ZeroWorldTo(UWorld* w, FVector newOrigin) {
	if (!w) {
		return;
	}

	w->SetNewWorldOrigin(FIntVector(newOrigin.X, newOrigin.Y, newOrigin.Z) + w->OriginLocation);
}


FHitResult UCustomExtensions::TraceSingle(UWorld* w, FVector from, FVector to, TArray<AActor*> ignored) {
	FHitResult r;
	FCollisionQueryParams params;
	params.AddIgnoredActors(ignored); 
	w->LineTraceSingleByChannel(r, from, to, ECollisionChannel::ECC_Visibility);
	return r;
}