// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceModeBase.h"



FString ARaceModeBase::GetOptions()
{
	return OptionsString;
}

FString ARaceModeBase::K2ParseOption(const FString& OptionString, const FString& OptionName)
{
	return  UGameplayStatics::ParseOption(OptionString, OptionName);
}

