// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MachGameMode.h"



FString AMachGameMode::GetOptions()
{
	return OptionsString;
}

FString AMachGameMode::K2ParseOption(const FString& OptionString, const FString& OptionName)
{
	return  UGameplayStatics::ParseOption(OptionString, OptionName);
}
