// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayFabLibrary.h"
#include "PlayFabCommon.h"
#include "BasicPlayFabSettings.h"
#include "PlayFab.h"
#include "Serialization/JsonWriter.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

bool UBasicPlayFabLibrary::IsLoggedIn()
{
	return true;
	//return IPlayFabModuleInterface::Get().GetClientAPI()->IsClientLoggedIn();
}

FString UBasicPlayFabLibrary::JsonToString(TSharedPtr<FJsonObject> Json)
{
	FString OutputString;

	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	return OutputString;
}
