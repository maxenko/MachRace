// Copyright 2015 - Max Enko

#include "SaveData.h"
#include "MachRace.h"


USaveData::USaveData(){
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USaveData::BeginPlay(){
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USaveData::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USaveData::PopulateWithDefaultValues() {
	ClientId = FGenericPlatformMisc::GetDeviceId();
	Callsign = "Unknown Pilot";
	LastSync = FDateTime::FromUnixTimestamp(0l);
	LastSaved = FDateTime::FromUnixTimestamp(0l);
	DateCreated = FDateTime::Now();
	TopSpeed = 0.0;
	Balance = 0l;

	Unlocked.Empty();
	Unlocked.Add("Ship01");
	Unlocked.Add("Ship02");

	LastSelectedShip = Unlocked[0];
}

FString USaveData::GerBalanceString() {
	return FString::Printf(TEXT("%llu"), Balance);
}

int64 USaveData::SetBalanceFromString(FString balanceAsStr) {
	Balance = FCString::Strtoi64(*balanceAsStr, NULL, 10);
	return Balance;
}

TArray<FString> USaveData::GetUnlockedShips() {
	TArray<FString> unlockedShips;

	for (FString ship : Unlocked) {
		if (ship.StartsWith("Ship")) {
			unlockedShips.Add(ship);
		}
	}
	
	return unlockedShips;
}

bool USaveData::CanUnlock(int32 price) {
	auto uPrice = (int64)price;

	return (price < Balance);
}

void USaveData::UpdateBalance(int32 by) {
	auto by64 = (int64)by;
	Balance += by64;
}
