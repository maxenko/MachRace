// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveData.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API USaveData : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USaveData();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FString ClientId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FString Callsign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FDateTime LastSync = FDateTime::FromUnixTimestamp(0l);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FDateTime DateCreated = FDateTime::Now();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FDateTime LastSaved = FDateTime::FromUnixTimestamp(0l);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	float TopSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	int64 Balance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	FString LastSelectedShip = "Ship01";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|SaveData")
	TArray<FString> Unlocked;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PopulateWithDefaultValues", Keywords = "Populate SaveData with default data."), Category = "MachRace|SaveData")
	void PopulateWithDefaultValues();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetBalanceAsString", Keywords = "Gets string value of Balance."), Category = "MachRace|SaveData")
	FString GerBalanceString();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetBalanceFromString", Keywords = "Sets string value of Balance."), Category = "MachRace|SaveData")
	int64 SetBalanceFromString(FString balanceAsStr);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetUnlockedShips", Keywords = "Filters Unlocked collection and returns only Ship* entries."), Category = "MachRace|SaveData")
	TArray<FString> GetUnlockedShips();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "CanUnlock", Keywords = "Checks wether balance can afford (unlock) given price."), Category = "MachRace|SaveData")
	bool CanUnlock(int32 price);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UpdateBalance", Keywords = "Update balance by given amount, positive number for deposit, negative for withdrawal."), Category = "MachRace|SaveData")
	void UpdateBalance(int32 by);
};
