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

        PrivateDependencyModuleNames.AddRange(new string[] { "VaRestPlugin" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
}
