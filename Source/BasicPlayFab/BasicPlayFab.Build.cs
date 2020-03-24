// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BasicPlayFab : ModuleRules
{
	public BasicPlayFab(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "PlayFab",
            "PlayFabCpp", 
            "PlayFabCommon", 
            "BasicPlayFab", 
            "Slate", 
            "SlateCore",
            "Core",
            "CoreUObject",
            "Engine",
            "UMG"
        });

        if (Target.bBuildEditor == true)
        {
            PublicDefinitions.Add("ENABLE_PLAYFABSERVER_API=1");
            PrivateDependencyModuleNames.AddRange(new string[] { "Settings" });
        }
    }
}
