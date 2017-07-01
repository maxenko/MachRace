// Copyright 2015 - Max Enko

using UnrealBuildTool;

public class MachRace : ModuleRules
{
	public MachRace(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "Http", 

            // steam
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        PrivateDependencyModuleNames.AddRange(new string[] { "VaRestPlugin","SodiumUE4" });
	}
}
