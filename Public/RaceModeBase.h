// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/GameMode.h"
#include "RaceModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API ARaceModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game", meta = (DisplayName = "GetOptions"))
	virtual FString GetOptions();

	UFUNCTION(BlueprintCallable, Category = "Game", meta = (DisplayName = "ParseOption"))
	virtual FString K2ParseOption(const FString& OptionString, const FString& OptionName);
};
