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
            "UMG",
            "Networking",
            "Sockets",
            "HTTP"
        });

        if (Target.bBuildEditor == true)
        {
            PublicDefinitions.Add("ENABLE_PLAYFABSERVER_API=1");
            PrivateDependencyModuleNames.AddRange(new string[] { "Settings" });
        }


        bUseRTTI = false;
        //DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
        if (Target.Type == TargetType.Server)
        {
            bEnableExceptions = true;
            // Add dynamic dlls required by all dedicated servers
            RuntimeDependencies.Add("$(TargetOutputDir)/xinput1_3.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/DirectX/xinput1_3.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/concrt140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/concrt140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_1.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_1.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_2.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_2.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/vccorlib140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vccorlib140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/vcruntime140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vcruntime140.dll", StagedFileType.SystemNonUFS);
            // Add dynamic dlls required by GSDK
            RuntimeDependencies.Add("$(TargetOutputDir)/GSDK_CPP_Windows.lib", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/GSDK_CPP_Windows.lib", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcurl.lib", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.lib", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcurl.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libssl-1_1-x64.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libssl-1_1-x64.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcrypto-1_1-x64.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcrypto-1_1-x64.dll", StagedFileType.SystemNonUFS);
            // Add libraries required by GSDK
            PublicAdditionalLibraries.Add("$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/GSDK_CPP_Windows.lib");
            PublicAdditionalLibraries.Add("$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.lib");
            PublicIncludePaths.Add("../../packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/include");
        }
    }
}
