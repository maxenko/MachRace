// Copyright 2015 - Max Enko

#include "RaceModeBase.h"
#include "MachRace.h"


FString ARaceModeBase::GetOptions()
{
	return OptionsString;
}


FString ARaceModeBase::K2ParseOption(const FString& OptionString, const FString& OptionName)
{
	return  UGameplayStatics::ParseOption(OptionString, OptionName);
}

