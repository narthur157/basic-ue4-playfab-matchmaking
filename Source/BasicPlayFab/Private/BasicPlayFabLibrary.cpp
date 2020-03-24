// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayFabLibrary.h"
#include "PlayFabCommon.h"
#include "BasicPlayFabSettings.h"
#include "PlayFab.h"

bool UBasicPlayFabLibrary::IsLoggedIn()
{
	return true;
	//return IPlayFabModuleInterface::Get().GetClientAPI()->IsClientLoggedIn();
}
