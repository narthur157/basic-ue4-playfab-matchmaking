// MIT License, Nick Arthur github.com/narthur157


#include "UI/BasicPlayFabMatchmake.h"
#include "PlayFab.h"
#include "Core/PlayFabClientAPI.h"
#include "PlayFabClientDataModels.h"

UBasicPlayFabMatchmake::UBasicPlayFabMatchmake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), PlayFabServerAPI(IPlayFabModuleInterface::Get().GetClientAPI())
{
	Region = PlayFab::ClientModels::Region::RegionUSEast;
}

void UBasicPlayFabMatchmake::FindMatch()
{
	PlayFab::ClientModels::FMatchmakeRequest Request;
	Request.BuildVersion = Build;
	Request.GameMode = Mode;
	Request.pfRegion = Region;

	auto Success = PlayFab::UPlayFabClientAPI::FMatchmakeDelegate::CreateLambda(
		[&](const PlayFab::ClientModels::FMatchmakeResult& Result)
	{
		OnMatchFound();

		FString Port = FString::FromInt(Result.ServerPort.mValue);
		UE_LOG(LogTemp, Log, TEXT("Match found, Lobby %s"), *Result.LobbyID);
		FString Url = Result.ServerIPV4Address + ":" + Port + "?ticket=" + Result.Ticket;
		
		GetOwningPlayer()->ClientTravel(Url, ETravelType::TRAVEL_Absolute, false);
	});

	auto Failure = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&](const PlayFab::FPlayFabCppError& Error)
	{
		OnMatchmakingFailure();
	});

	PlayFabServerAPI->Matchmake(Request, Success, Failure);
}
