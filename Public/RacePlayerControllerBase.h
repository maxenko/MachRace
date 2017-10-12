// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/PlayerController.h"
#include "CustomExtensions.h"
#include "RaceShipBase.h"
#include "RacePlayerControllerBase.generated.h"

UCLASS()
class MACHRACE_API ARacePlayerControllerBase : public APlayerController {
	GENERATED_BODY()


public:

	ARacePlayerControllerBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	UCurveFloat* BankingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool InputRightEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool InputLeftEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool DevHacksEnabled = false;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetState", Keywords = "Gets game specific state."), Category = "MachRace|Utility")
	ARaceGameStateBase* GetState();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetShip", Keywords = "Gets ship reference to current ship."), Category = "MachRace|Utility")
	ARaceShipBase* GetShip(bool &success);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bank", Keywords = "Bank ship."), Category = "MachRace|Engine")
	void Bank(float alpha);
};