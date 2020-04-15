// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IBasicPlayFab.h"
#if WITH_EDITOR
#include <ISettingsModule.h>
#include <ISettingsContainer.h>
#include <ISettingsSection.h>
#endif

#if UE_SERVER
#include "gsdk.h"
#include "string.h"
#endif

#include "PlayFabRuntimeSettings.h"
#include "Internationalization/Internationalization.h"
#include "BasicPlayFabSettings.h"

#define LOCTEXT_NAMESPACE "BasicPlayFab"

DEFINE_LOG_CATEGORY(BasicPlayFab);

class FBasicPlayFabModule : public IBasicPlayFab
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~ End IModuleInterface

#if UE_SERVER
	void StartServer();
	/*void LogInfo(FString message);
	void LogError(FString message);*/
#endif

	void RegisterSettings();
	void UnregisterSettings();
	bool HandleSettingsSaved();
};

IMPLEMENT_MODULE(FBasicPlayFabModule, BasicPlayFab)

// Based off of this https://wiki.unrealengine.com/CustomSettings
void FBasicPlayFabModule::RegisterSettings() 
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Create the new category
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");
		
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Basic PlayFab",
			LOCTEXT("BasicPlayFabSettingsName", "Basic Playab"),
			LOCTEXT("BasicPlayFabSettingsDescription", "Configure Basic PlayFab Settings"),
			GetMutableDefault<UBasicPlayFabSettings>()
		);

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FBasicPlayFabModule::HandleSettingsSaved);
		}
	}
#endif
}

void FBasicPlayFabModule::UnregisterSettings()
{
#if WITH_EDITOR
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		//SettingsModule->UnregisterSettings("Project", "BasicPlayFab", "BasicPlayFab");
	}
#endif
}

bool FBasicPlayFabModule::HandleSettingsSaved()
{
#if WITH_EDITOR
	UBasicPlayFabSettings* Settings = GetMutableDefault<UBasicPlayFabSettings>();
	bool ResaveSettings = false;

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}
#endif

	return true;
}

#if UE_SERVER

void LogInfo(FString message)
{
	UE_LOG(BasicPlayFab, Display, TEXT("%s"), *message);
	Microsoft::Azure::Gaming::GSDK::logMessage(std::string(TCHAR_TO_UTF8(*message)));
}

void LogError(FString message)
{
	UE_LOG(BasicPlayFab, Error, TEXT("%s"), *message);
	Microsoft::Azure::Gaming::GSDK::logMessage(std::string(TCHAR_TO_UTF8(*message)));
}

void OnShutdown()
{
	LogInfo("Shutting down");
	FGenericPlatformMisc::RequestExit(true);
}

bool HealthCheck()
{
	LogInfo("Healthy");
	return true;
}

void FBasicPlayFabModule::StartServer()
{
	try {
		Microsoft::Azure::Gaming::GSDK::start();
		Microsoft::Azure::Gaming::GSDK::registerHealthCallback(&HealthCheck);
		Microsoft::Azure::Gaming::GSDK::registerShutdownCallback(&OnShutdown);

		if (Microsoft::Azure::Gaming::GSDK::readyForPlayers())
		{
			LogInfo("Server is ready for players");
		}
		else
		{
			LogError("Server is terminating. Not ready for players");
		}
	}
	catch (Microsoft::Azure::Gaming::GSDKInitializationException& e)
	{
		LogError("GSDK Initialization failed: " + FString(UTF8_TO_TCHAR(e.what())));
	}
}
#endif

void FBasicPlayFabModule::StartupModule()
{
	RegisterSettings();

#if UE_SERVER
	if (FParse::Param(FCommandLine::Get(), TEXT("NoPlayFab")))
	{
		StartServer();
	}
#else
	UE_LOG(BasicPlayFab, Display, TEXT("Not UE_SERVER, in StartupModule()"))
#endif
}

void FBasicPlayFabModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

