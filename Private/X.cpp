// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Algo/Reverse.h"
#include "Kismet/KismetMathLibrary.h"

FVector UX::GetRootLinearVelocity(AActor* target) {
	auto targetRoot = Cast<UPrimitiveComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsLinearVelocity();
}

FVector UX::GetRootAngularVelocity(AActor* target) {
	auto targetRoot = Cast<UPrimitiveComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsAngularVelocityInRadians();
}

void UX::SetRootLinearVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UPrimitiveComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsLinearVelocity(v, addTo);
}

void UX::SetRootAngularVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UPrimitiveComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsAngularVelocityInRadians(v, addTo);
}


FVector UX::NullifyX(FVector v) {
	return FVector(0, v.Y, v.Z);
}

FVector UX::NullifyY(FVector v) {
	return FVector(v.X, 0, v.Z);
}

FVector UX::NullifyZ(FVector v) {
	return FVector(v.X, v.Y, 0);
}

UPrimitiveComponent* UX::GetRootAsPrimitive(AActor* a, bool& success) {

	if (!a) {
		success = false;
		return NULL;
	}

	auto rootAsPrimitive = Cast<UPrimitiveComponent>(a->GetRootComponent());

	if (rootAsPrimitive) {
		success = true;
		return rootAsPrimitive;
	} else {
		success = false;
	}

	return NULL;
}

void UX::DecayRootRotToZero(AActor* a, float delta, float decaySpeed) {
	bool rootOk = false;
	auto physVol = UX::GetRootAsPrimitive(a,rootOk);

	if (!rootOk) {
		return;
	}

	// stop all rotation
	if (physVol->GetPhysicsAngularVelocityInRadians() != FVector::ZeroVector) {
		physVol->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	}

	physVol->SetWorldRotation(FMath::RInterpTo(physVol->GetComponentRotation(), FRotator(0, 0, 0), delta, decaySpeed),false,nullptr,ETeleportType::TeleportPhysics);
}

bool UX::VectorsWithinAngle(FVector a, FVector b, float angle) {
	// calc angle between forward and hit vector

	a.Normalize();
	b.Normalize();

	auto angeToEachOther = FVector::DotProduct(a, b);
	auto deg = UKismetMathLibrary::DegCos(angeToEachOther);

	return deg < angle;
}

bool UX::IsWithinCameraView(APlayerCameraManager* cm, FVector to, float within) {

	if (!cm) {
		return false;
	}

	auto cameraLoc = cm->GetCameraLocation();
	auto cameraDir = cm->GetCameraRotation().Vector();
	cameraDir.Normalize();

	FVector directional = cameraLoc - to; // to target normal
	directional.Normalize();

	// angle between vectors (we're concerned whether or not vector falls inside the visible cone & dist
	float dProductAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(cameraDir, directional))); // angle to target

	return FMath::Abs(within) > FMath::Abs(180 - dProductAngle);
}

float UX::SnapToInterval(float interval, float v) {
	
	float multiplier = v < 0 ? -1 : 1;

	float absI = FMath::Abs(interval);
	float absV = FMath::Abs(v);

	if (absV < absI) {
		return 0;
	}

	float remainder = FMath::Fmod(absV, absI);
	return v - remainder * multiplier;
}

FVector UX::RandVecInRange(FVector min, FVector max) {

	auto X = FMath::FRandRange(min.X, max.X);
	auto Y = FMath::FRandRange(min.Y, max.Y);
	auto Z = FMath::FRandRange(min.Z, max.Z);

	return FVector(X, Y, Z);
}

FRotator UX::GetTargetLookAtRot(FTransform actorT, FVector targetLoc) {
	auto direction = -actorT.InverseTransformPosition(targetLoc);
	FRotator rot = FRotationMatrix::MakeFromX(direction).Rotator();
	return rot;
}

float UX::GetYDistBetweenActors(AActor* a, AActor* b) {

	 
	if (!a || !b) {
		return -1;
	}

	// strip out X,Z
	auto locA = FVector(0, a->GetActorLocation().Y, 0);
	auto locB = FVector(0, b->GetActorLocation().Y, 0);

	return FVector::Dist(locA, locB);
}

TArray<UObject*> UX::CullAtRandom(TArray<UObject*> a, int32 numberToCull) {

	// sanity check
	if (numberToCull == 0 || a.Num() == 0) {
		return a;
	}

	TArray<UObject*> b;
	b.Append(a);

	for (int32 i = 0; i < numberToCull; ++i) {

		if (b.Num() <= 0) {
			break;
		}

		b.RemoveAt(FMath::RandRange(0, b.Num() - 1));
	}

	return b;
}

float UX::PickRandomFloatFrom(TArray<float> floats) {
	int32 randIdx = FMath::RandRange(0, floats.Num() - 1);
	return floats[randIdx];
}

void UX::StoreIntSetting(FString category, FString settingName, int32 setting) {

	if (!GConfig) { return; }

	GConfig->SetInt(
		*category,
		*settingName,
		setting,
		GGameIni
	);
}

void UX::StoreStringSetting(FString category, FString settingName, FString setting) {

	if (!GConfig) { return; }

	GConfig->SetString(
		*category,
		*settingName,
		*setting,
		GGameIni
	);
}

int32 UX::GetIntSetting(FString category, FString settingName) {

	int32 setting = -1;

	GConfig->GetInt(
		*category,
		*settingName,
		setting,
		GGameIni
	);

	return setting;
}

FString UX::GetStringSetting(FString category, FString settingName) {

	FString setting = "";

	GConfig->GetString(
		*category,
		*settingName,
		setting,
		GGameIni
	);

	return setting;
}

float UX::DistSansX(FVector from, FVector to) {
	return FVector::Dist(FVector(0, from.Y, to.Z), FVector(0, to.Y, to.Z));
}



ARaceGameStateBase* UX::GetRaceState(UWorld* w) {
	if (!w) {
		return NULL;
	}

	auto _state = w->GetGameState();
	ARaceGameStateBase* state = Cast<ARaceGameStateBase>(_state);
	return state;
}

float UX::DistSansZ(FVector from, FVector to) {
	return FVector::Dist(FVector(from.X, from.Y, 0), FVector(to.X, to.Y, 0));

}

TArray<FVector> UX::GenerateRandomPointsWithinBounds(
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

		points.Add(FVector(x_rand, y_rand, z_rand) + origin);
	}

	return points;
}


void UX::SetMotionBlur(UCameraComponent* cam, float amount, float max) {
	cam->PostProcessSettings.MotionBlurAmount = amount;
	cam->PostProcessSettings.MotionBlurMax = max;
}

void UX::SetSaturation(UCameraComponent* cam, float amount) {
	cam->PostProcessSettings.FilmSaturation = amount;
}

void UX::SetVignetteIntensity(UCameraComponent* cam, float amount) {
	cam->PostProcessSettings.VignetteIntensity = amount;
}

void UX::SetLensFlareIntensity(UCameraComponent* cam, float amount) {
	auto intensity = amount < 0.0 ? 0.0 : amount;
	cam->PostProcessSettings.LensFlareIntensity = intensity;
}

FVector UX::AtoBOffset(FVector a, FVector b) {
	auto diff = a - b;

	return FVector(
		FMath::Abs(diff.X),
		FMath::Abs(diff.Y),
		FMath::Abs(diff.Z)
	);
}

FVector UX::ExtentToSize(FVector v) {

	auto absv = v.GetAbs() * 2;

	return FVector(
		absv.X, absv.Y, absv.Z
	);
}

float UX::RemapValueToRange(float val, float oldRangeMin, float oldRangeMax, float newRangeMin, float newRangeMax) {
	return (((val - oldRangeMin) * (newRangeMax - newRangeMin)) / (oldRangeMax - oldRangeMin)) + newRangeMin;
}

bool UX::OverlapsPoint2D(FVector2D v, float xMin, float xMax, float yMin, float yMax) {
	return v.X >= xMin && v.X <= xMax && v.Y >= yMin && v.Y >= yMax;
}

bool UX::OverlapsPoint3D(FVector point, FVector origin, FVector bounds) {

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

bool UX::OverlapsPoint3DSansZ(FVector point, FVector origin, FVector bounds) {

	float xMin = origin.X - bounds.X;
	float xMax = origin.X + bounds.X;
	float yMin = origin.Y - bounds.Y;
	float yMax = origin.Y + bounds.Y;

	return
		point.X >= xMin && point.X <= xMax &&
		point.Y >= yMin && point.Y <= yMax;
}

TArray<FVector> UX::FilterPointsThatOverlapOtherPoints(TArray<FVector> toFilter, TArray<FVector> toFilterAgainst, float minDist, bool ignoreZ) {
	TArray<FVector> filtered;

	auto isTooColose = [&](FVector point) {
		for (auto tfa : toFilterAgainst) {
			if (!ignoreZ) {
				if (FVector::Dist(point, tfa) < minDist) {
					return true;
				}
			} else {
				if (UX::DistSansZ(point, tfa) < minDist) {
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

bool UX::AnyBoundsOverlapPoint(FVector point, TArray<FVector> origins, TArray<FVector> bounds) {

	for (int32 i = 0; i < origins.Num() && i < bounds.Num(); ++i) {
		if (UX::OverlapsPoint3D(point, origins[i], bounds[i])) {
			return true;
		}
	}

	return false;
}

TArray<FTransform> UX::FilterTransformsByMinDistance(TArray<FTransform> points, float dist) {

	auto filtered = TArray < FTransform >();
	auto tooClose = TArray < FTransform >();

	for (const FTransform& pOuter : points) {
		for (const FTransform& pInner : points) {

			auto tInner = pInner.GetTranslation();
			auto tOuter = pOuter.GetTranslation();

			if (!FVector::PointsAreSame(tInner, tOuter) && FVector::PointsAreNear(tInner, tOuter, dist)) {
				tooClose.Add(pInner);
			}
		}
	}

	for (const FTransform& p : points) {

		bool contains = false;
		for (const FTransform& t : tooClose) {
			if (t.GetTranslation() == p.GetTranslation()) {
				contains = true;
			}
		}

		if (!contains) {
			filtered.Add(p);
		}
	}

	return filtered;
}

TArray<FVector> UX::FilterPointsByMinDistance(TArray<FVector> points, float dist) {

	auto filtered = TArray < FVector >();
	auto tooClose = TArray < FVector >();

	for (const FVector& pOuter : points) {
		for (const FVector& pInner : points) {
			if (!FVector::PointsAreSame(pInner, pOuter) && FVector::PointsAreNear(pInner, pOuter, dist)) {
				tooClose.Add(pInner);
			}
		}
	}

	for (const FVector& p : points) {
		if (!tooClose.Contains(p)) {
			filtered.Add(p);
		}
	}

	return filtered;
}

TArray<FVector> UX::FilterPointsOverlappingBounds(TArray<FVector> points, FVector boundsOrigin, FVector bounds) {

	auto nonOverlappingPoints = TArray < FVector >();

	for (const FVector& p : points) {
		if (!UX::OverlapsPoint3D(p, boundsOrigin, bounds)) {
			nonOverlappingPoints.Add(p);
		}
	}

	return nonOverlappingPoints;
}

TArray<FVector>  UX::FilterPointsInsideActorBounds(TArray<FVector> points, TArray<AActor*> actors) {

	auto filteredPoints = TArray < FVector >(points);

	for (const AActor* a : actors) {
		FVector o;
		FVector e;
		a->GetActorBounds(false, o, e);
		filteredPoints = UX::FilterPointsOverlappingBounds(filteredPoints, o, e);
	}

	return filteredPoints;

}

int32 UX::WrapIdxTo(int32 Len, int32 index) {

	if (index == 0) {
		return index;
	}

	return ((index*(index - Len - (index % Len)) / abs(index) + index + Len + (index % Len)) >> 1) % Len;

}

FText UX::SubstrAlpha(FString s, int32 len, float alpha, bool& atEnd) {

	alpha = FMath::Clamp((float)alpha, (float)0, (float) 1.0);
	len = FMath::Clamp((int32)len, (int32)0, (int32)s.Len());

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

TArray<FString> UX::ArraySlice(const TArray<FString>& TargetArray, int32 Index, int32 Size) {

	TArray<FString> slice;
	auto to = Index + Size;

	if (Size == 0 || to > TargetArray.Num()) {
		slice.Empty();
		return slice;
	}

	Index = FMath::Clamp(Index, 0, TargetArray.Num());
	Size = FMath::Clamp(Size, 0, TargetArray.Num() - 1);

	for (auto i = Index; i < to; ++i) {
		slice.Add(TargetArray[i]);
	}

	return slice;
}

TArray<float> UX::RandomSortedFloats(int32 Length, float Min, float Max) {

	TArray<float> list;

	for (auto i = 0; i < Length; ++i) {
		list.Add(FMath::FRandRange(Min, Max));
	}

	list.Sort();
	return list;
}

int32 UX::SumIntArray(TArray<int32> a) {
	int32 sum = 0;
	for (int32 i : a) {
		sum += i;
	}
	return sum;
}

TArray<int32> UX::Tail(TArray<int32> a) {

	TArray<int32> tail;

	// if array length is 1 or 0, return empty tail
	if (a.Num() <= 1) {
		return tail;
	}

	for (auto i = 1; i < a.Num(); ++i) {
		tail.Add(a[i]);
	}

	return tail;
}

int32 UX::PickRandomIdxByWeight(TArray<int32> weights) {

	if (weights.Num() == 0) {
		return 0;
	}

	TArray<int32> unsorted;
	unsorted.Append(weights); // keep unsorted version
	weights.Sort(); // we sort values to match sequentially below
	auto sum = UX::SumIntArray(weights);
	auto rndWeight = FMath::RandRange(0, sum);

	for (auto i = 0; i < weights.Num(); ++i) {
		if (rndWeight < weights[i]) {
			return unsorted.Find(weights[i]); // return unsorted index
		}
	}

	return 0; // should never reach this!
}

FString UX::UrlEncode(const FString url) {
	return FGenericPlatformHttp::UrlEncode(url);
}

TArray<FString> UX::GetFakeScores(TArray<FString> names, float pct, int32 size) {

	TArray<FString> scores;
	auto ctr = 0;
	auto maxspeed = 27312;
	pct = FMath::Clamp((float)pct, (float)0, (float) 100.0);

	for (FString name : names) {

		FString rank = FString::Printf(TEXT("%d"), ++ctr);
		maxspeed -= 17;
		auto mph = FString::Printf(TEXT("%d"), maxspeed);

		FString formatted = FText::Format(LOCTEXT("ScoreFormat", "{0};{1};{2}"), FText::FromString(rank), FText::FromString(mph), FText::FromString(name)).ToString();

		scores.Add(formatted);

	}

	int32 pageStart = FMath::TruncToInt(UX::RemapValueToRange(pct, 0.0, 100.0, 0.0, (float)scores.Num()));

	if (pageStart + size > scores.Num()) {
		return UX::ArraySlice(scores, scores.Num() - size, size);
	} else {
		return UX::ArraySlice(scores, pageStart, size);
	}

}

TArray<FVector> UX::GetSpacedRandomVectors(float maxDistToEachOther, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, int32 count, int32 maxTries) {

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

float UX::AffectorWeight(FVector from, FVector to, float range, float faloff) {

	auto dist = FVector::Dist(from, to);

	if (dist > range) {

		return 0.0;

	} else if (dist < faloff) {

		return  1.0;

	} else {

		auto rangeFrom = FVector2D(range, faloff);
		auto rangeTo = FVector2D(0.0, 1.0);
		return FMath::GetMappedRangeValueClamped(rangeFrom, rangeTo, dist);

	}
}

int32 UX::Level1ObstacleCount(float speed) {
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

int32 UX::Level1CableCount(float speed) {

	if (speed < 2600) {
		return FMath::RandRange(2, 5);;
	} else {
		return 0;
		//return FMath::RandRange(0, 0);
	}

	return 0;
}

TArray<FVector> UX::GenSkewedHexagonGrid(int32 columns, int32 rows, float size) {

	// flat top orientation

	TArray<FVector> coords;
	float height = size * 2;
	float width = FMath::Sqrt(3) / 2 * height;
	float hOffset = width / 2;

	auto vOffset = [height](int row) { return row == 0 ? 0 : height * .25; }; // 1st row is 1/3 of the offset

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < columns; ++col) {

			float x = col * width + hOffset;
			float y = height / 2 + vOffset(row) * row;

			coords.Add(FVector(x, y, 0));
		}

		hOffset += width / 2;
	}

	return coords;
}

TArray<FTransform> UX::GenSkewedHexagonGridTransforms(int32 columns, int32 rows, float size, float multiplier, FVector2D verticalJitterRange) {
	auto vs = UX::GenSkewedHexagonGrid(columns, rows, size);

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

TArray<FTransform> UX::MakeTemplatedTransforms(TArray<FVector>vectors, FTransform t) {
	TArray<FTransform> ret;

	for (auto v : vectors) {
		ret.Add(
			FTransform(t.GetRotation(), v, t.GetScale3D())
		);
	}

	return ret;
}

TArray<FVector> UX::ExtractLocations(TArray<FTransform>transforms) {

	TArray<FVector> vs;
	for (FTransform t : transforms) {
		vs.Add(t.GetLocation());
	}

	return vs;
}

FTransform UX::RandomTWithMax(FTransform t) {
	return FTransform(


	);
}

FVector2D UX::GetSkewedHexGridDimensions(int32 cols, int32 rows, float cellSize) {

	float height = cellSize * 2;
	float width = FMath::Sqrt(3) / 2 * height;
	float oddEvenOffset = (FMath::Sqrt(3) / 2 * height) / 4;
	float horisontalOffset = (FMath::Sqrt(3) / 2 * height) * (FMath::Sqrt(2.5) / 4);

	if (cols % 2 == 0) {
		oddEvenOffset = -oddEvenOffset;
		horisontalOffset = -width / 8;
	}

	return FVector2D(
		rows * height / 2 + width / 2,
		cols * width / 2 + horisontalOffset
	);

}

bool UX::IsInRange(float Value, float Min, float Max) {
	return ((true ? (Value >= Min) : (Value > Min)) && (true ? (Value <= Max) : (Value < Max)));
}


FLinearColor UX::InterpLinearColor(FLinearColor current, FLinearColor to, float delta, float speed) {
	FLinearColor color;

	color.R = UKismetMathLibrary::FInterpTo(current.R, to.R, delta, speed);
	color.G = UKismetMathLibrary::FInterpTo(current.G, to.G, delta, speed);
	color.B = UKismetMathLibrary::FInterpTo(current.B, to.B, delta, speed);
	color.A = UKismetMathLibrary::FInterpTo(current.A, to.A, delta, speed);

	return color;
}

FString UX::GetStageName(GameStage s) {
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

bool UX::IsShip(AActor* actor) {
	return actor->IsA(ARaceShipBase::StaticClass());
}

void UX::ZeroWorldTo(UWorld* w, FVector newOrigin) {
	if (!w) {
		return;
	}

	w->SetNewWorldOrigin(FIntVector(newOrigin.X, newOrigin.Y, newOrigin.Z) + w->OriginLocation);
}

FHitResult UX::TraceSingle(UWorld* w, FVector from, FVector to, TArray<AActor*> ignored) {
	FHitResult r;
	FCollisionQueryParams params;
	params.AddIgnoredActors(ignored);
	w->LineTraceSingleByChannel(r, from, to, ECollisionChannel::ECC_Visibility);
	return r;
}

void UX::SplitArray(TArray<uint8> input, int32 index, bool& success, TArray<uint8>& a, TArray<uint8>& b) {

	if (!input.IsValidIndex(index)) {
		success = false;
		return;
	}

	a.Empty();
	b.Empty();

	a.Append(input.GetData(), index);
	input.RemoveAt(0, index);
	b.Append(input);

	success = true;
}

FVector UX::ReadjustVector(FVector vec, FVector from, float minDist) {

	auto dist = FVector::Distance(vec, from);

	if (dist >= minDist) {
		// already at minimum distance
		return vec;
	} else {

		// space out the vector

		auto direction = (vec - from).GetSafeNormal();

		return (vec + (direction * (minDist - dist)));
	}
}

//////////////////////////////////////////////////////////////////////////
// IO
//////////////////////////////////////////////////////////////////////////

FString UX::GetSavePath() {
	return FPaths::ProjectUserDir();
}

//If this function cannot find or create the directory, returns false.
bool UX::TouchDir(const FString& dirPath) {

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Directory Exists?
	if (!PlatformFile.DirectoryExists(*dirPath)) {
		PlatformFile.CreateDirectory(*dirPath);

		if (!PlatformFile.DirectoryExists(*dirPath)) {
			return false;
		}
	}

	return true;
}

bool UX::SaveData(TArray<uint8> data, FString dataName) {
	auto fileName = dataName + ".race";
	auto fullPath = UX::GetSavePath() + fileName;

	auto success = FFileHelper::SaveArrayToFile(data, *fullPath);
	data.Empty();

	return success;
}

bool UX::LoadData(TArray<uint8>& data, FString dataName) {

	auto fileName = dataName + ".race";
	auto fullPath = UX::GetSavePath() + fileName;

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (!platformFile.FileExists(*fullPath)) {
		return false;
	}

	return FFileHelper::LoadFileToArray(data, *fullPath);
}

bool UX::DataExists(const FString& dataName) {
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	auto fileName = dataName + ".race";
	auto fullPath = UX::GetSavePath() + fileName;
	return platformFile.FileExists(*fullPath);
}