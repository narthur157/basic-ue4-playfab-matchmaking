// MIT License, Nick Arthur github.com/narthur157

#include "BasicMatchmakingSystem.h"
#include "PlayFab.h"
#include "PlayFabCommon.h"
#include "PlayFabError.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "Core/PlayFabMultiplayerInstanceAPI.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "PlayFabAuthenticationDataModels.h"
#include "TimerManager.h"
#include "Dom/JsonObject.h"
#include "BasicPlayFabLibrary.h"

UBasicMatchmakingSystem::UBasicMatchmakingSystem(const FObjectInitializer& ObjectInitializer)
	: Super(), PlayFabClientAPI(IPlayFabModuleInterface::Get().GetClientAPI()), PlayFabMultiplayerAPI(IPlayFabModuleInterface::Get().GetMultiplayerAPI())
{
	ErrorDelegate = PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UBasicMatchmakingSystem::HandleMatchmakingFailure);
}

bool UBasicMatchmakingSystem::FindMatch()
{
	if (bLookingForMatch)
	{
		return false;
	}

	if (CurrentPlayFabId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot start matchmaking when not logged in"));
		return false;
	}

	bLookingForMatch = true;

	auto Success = PlayFab::UPlayFabMultiplayerInstanceAPI::FCreateMatchmakingTicketDelegate::CreateLambda(
		[&](const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("FindMatch succeeded, polling match status.."));
		CurrentTicket = Result.TicketId;

		PollMatchmakingTicket();
	});

	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest Request;

	Request.GiveUpAfterSeconds = 120;
	Request.QueueName = QueueMode;

	TSharedPtr<FJsonObject> AttributesObject = MakeMatchmakingAttributes();

	UE_LOG(LogTemp, Log, TEXT("Made matchmaking attributes:\n%s"), *UBasicPlayFabLibrary::JsonToString(AttributesObject));
	Request.Creator.Entity = MakePlayerEntity();

	Request.Creator.Attributes = MakeShared<PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes>(PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes(AttributesObject));

	PlayFabMultiplayerAPI->CreateMatchmakingTicket(Request, Success, ErrorDelegate);
	
	return true;
}

void UBasicMatchmakingSystem::SetActivePlayFabId(const FString& Id)
{
	CurrentPlayFabId = Id;
}

void UBasicMatchmakingSystem::PollMatchmakingTicket()
{
	if (!bLookingForMatch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not polling because matchmaking is not in progress"));
		return;
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (!TimerManager.IsTimerActive(PollingTimer))
	{
		TimerManager.SetTimer(PollingTimer, this, &UBasicMatchmakingSystem::PollMatchmakingTicket, 6.0f, true);
	}

	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;

	Request.TicketId = CurrentTicket;
	Request.QueueName = QueueMode;

	auto Success = PlayFab::UPlayFabMultiplayerInstanceAPI::FGetMatchmakingTicketDelegate::CreateLambda(
		[&](const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result)
	{
		
		if (!(Result.MatchId.IsEmpty()))
		{
			HandleMatchFound(Result.MatchId);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Polling, no match found yet"));
		}
	});

	PlayFabMultiplayerAPI->GetMatchmakingTicket(Request, Success, ErrorDelegate);
}

void UBasicMatchmakingSystem::HandleMatchmakingFailure(const PlayFab::FPlayFabCppError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *Error.GenerateErrorReport());
	bLookingForMatch = false;
	GetWorld()->GetTimerManager().ClearTimer(PollingTimer);
	
	OnMatchmakingError.Broadcast(Error.ErrorMessage);

	PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerRequest Request;
	Request.Entity = MakeShared<PlayFab::MultiplayerModels::FEntityKey>(MakePlayerEntity());
	Request.QueueName = QueueMode;

	PlayFabMultiplayerAPI->CancelAllMatchmakingTicketsForPlayer(Request);
}

void UBasicMatchmakingSystem::HandleMatchFound(FString MatchId)
{
	PlayFab::MultiplayerModels::FGetMatchRequest Request;
	Request.MatchId = MatchId;

	auto Success = PlayFab::UPlayFabMultiplayerInstanceAPI::FGetMatchDelegate::CreateLambda(
		[&](const PlayFab::MultiplayerModels::FGetMatchResult& Result)
	{
		bLookingForMatch = false;
		GetWorld()->GetTimerManager().ClearTimer(PollingTimer);
		
		FBasicMatchmakingResult BasicResult = FBasicMatchmakingResult(Result.pfServerDetails);
		
		FString Url = BasicResult.IpAddress + ":" + BasicResult.Port;	// + "?ticket=" + Result.Ticket;
		BasicResult.Url = Url;

		OnMatchFound.Broadcast(Result.pfServerDetails);
	});

	PlayFabMultiplayerAPI->GetMatch(Request, Success, ErrorDelegate);
}

TSharedPtr<FJsonObject> UBasicMatchmakingSystem::MakeMatchmakingAttributes()
{
	FJsonObject AttributesObject;
	FJsonObject DataObject;

	TArray<TSharedPtr<FJsonValue>> Latencies;
	TSharedPtr<FJsonObject> LatencyObject = MakeShared<FJsonObject>();
	TSharedPtr<FJsonObject> LatencyObject2 = MakeShared<FJsonObject>();


	// According to this, this is necessary? https://docs.microsoft.com/en-us/gaming/playfab/features/multiplayer/matchmaking/ticket-attributes
	LatencyObject->SetStringField("region", "RegionUSEast");
	LatencyObject->SetNumberField("latency", 100);

	LatencyObject2->SetStringField("region", "EastUs2");
	LatencyObject2->SetNumberField("latency", 200);
	
	Latencies.Add(MakeShared<FJsonValueObject>(FJsonValueObject(LatencyObject)));
	Latencies.Add(MakeShared<FJsonValueObject>(FJsonValueObject(LatencyObject2)));
	
	DataObject.SetArrayField("region", Latencies);
	DataObject.SetArrayField("Latencies", Latencies);

	AttributesObject.SetObjectField("DataObject", MakeShared<FJsonObject>(DataObject));

	return MakeShared<FJsonObject>(AttributesObject);
}

PlayFab::MultiplayerModels::FEntityKey UBasicMatchmakingSystem::MakePlayerEntity()
{
	PlayFab::MultiplayerModels::FEntityKey Entity;

	Entity.Id = CurrentPlayFabId; //IPlayFabCommonModuleInterface::Get().GetEntityToken(); //PlayFabCommon::PlayFabCommonSettings::entityToken;
	Entity.Type = "title_player_account";

	return Entity;
}

FBasicMatchmakingResult::FBasicMatchmakingResult(TSharedPtr<PlayFab::MultiplayerModels::FServerDetails> PlayFabServerDetails, FString Url)
	: FBasicMatchmakingResult(PlayFabServerDetails)
{
	this->Url = Url;
}

FBasicMatchmakingResult::FBasicMatchmakingResult(TSharedPtr<PlayFab::MultiplayerModels::FServerDetails> PlayFabServerDetails)
	: IpAddress(PlayFabServerDetails->IPV4Address)
{
	Port = FString::FromInt(PlayFabServerDetails->Ports[0].Num);
}
