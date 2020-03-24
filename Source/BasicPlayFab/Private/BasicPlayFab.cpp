// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IBasicPlayFab.h"
#if WITH_EDITOR
#include <ISettingsModule.h>
#include <ISettingsContainer.h>
#include <ISettingsSection.h>
#endif
#include "PlayFabRuntimeSettings.h"
#include "Internationalization/Internationalization.h"
#include "PlayFabCommon.h"

#define LOCTEXT_NAMESPACE "BasicPlayFab"

class FBasicPlayFabModule : public IBasicPlayFab
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

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

		SettingsContainer->DescribeCategory("BasicPlayFab",
			LOCTEXT("Basic PlayFab Settings", "BasicPlayFab"),
			LOCTEXT("Basic PlayFab Settings", "Setup for PlayFab"));
		
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "BasicPlayFab", "BasicPlayFab",
			LOCTEXT("BasicPlayFab Settings", "BasicPlayFab"),
			LOCTEXT("BasicPlayFab Settings", "PlayFab Runtime Settings"),
			GetMutableDefault<UPlayFabRuntimeSettings>()
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
	UPlayFabRuntimeSettings* Settings = GetMutableDefault<UPlayFabRuntimeSettings>();
	bool ResaveSettings = false;

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

#endif
	return true;
}


void FBasicPlayFabModule::StartupModule()
{
	RegisterSettings();
}

void FBasicPlayFabModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

