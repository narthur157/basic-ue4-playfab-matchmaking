// MIT License, Nick Arthur github.com/narthur157

#include "BasicMatchmakingSystem.h"
#include "BasicPlayFabLibrary.h"
#include "BasicPlayFabSettings.h"

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

UBasicMatchmakingSystem::UBasicMatchmakingSystem(const FObjectInitializer& ObjectInitializer)
	: Super(), PlayFabClientAPI(IPlayFabModuleInterface::Get().GetClientAPI()), PlayFabMultiplayerAPI(IPlayFabModuleInterface::Get().GetMultiplayerAPI())
{
	ErrorDelegate = PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UBasicMatchmakingSystem::HandleMatchmakingFailure);
}

void UBasicMatchmakingSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if UE_SERVER
	UBasicPlayFabSettings* Settings = UBasicPlayFabSettings::Get();

	if (Settings->bUseMaxGameLength)
	{
		FTimerHandle MaxGameLengthTimer;
		GetWorld()->GetTimerManager().SetTimer(MaxGameLengthTimer, [&]() {
			FGenericPlatformMisc::RequestExit(true);
		}, Settings->MaxGameLength, false);
	}
#endif
}

void UBasicMatchmakingSystem::Deinitialize()
{
	Super::Deinitialize();

	// Try to clear out any tickets on our way out so that we aren't put into a match we can't join
	CancelMatchmaking();
}

bool UBasicMatchmakingSystem::FindMatch()
{
	if (bLookingForMatch)
	{
		return false;
	}

	if (CurrentPlayerEntityId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot start matchmaking when not logged in"));
		return false;
	}

	bLookingForMatch = true;

	auto Success = PlayFab::UPlayFabMultiplayerInstanceAPI::FCreateMatchmakingTicketDelegate::CreateLambda(
		[&](const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("CreateMatchmakingTicket succeeded, polling match status.."));
		CurrentTicket = Result.TicketId;

		PollMatchmakingTicket();
	});

	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest Request;

	Request.GiveUpAfterSeconds = UBasicPlayFabSettings::Get()->MatchmakingTimeout;
	Request.QueueName = QueueMode;
	Request.MembersToMatchWith = {};

	TSharedPtr<FJsonObject> AttributesObject = MakeMatchmakingAttributes();

	UE_LOG(LogTemp, Log, TEXT("Made matchmaking attributes:\n%s"), *UBasicPlayFabLibrary::JsonToString(AttributesObject));

	Request.Creator.Entity = MakePlayerEntity();
	Request.Creator.Attributes = MakeShared<PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes>(PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes(AttributesObject));

	UE_LOG(LogTemp, Log, TEXT("CreateMatchMakingRequest: %s"), *Request.toJSONString());
	PlayFabMultiplayerAPI->CreateMatchmakingTicket(Request, Success, ErrorDelegate);
	
	return true;
}

bool UBasicMatchmakingSystem::IsMatchmakingActive()
{
	return bLookingForMatch;
}

void UBasicMatchmakingSystem::SetActivePlayFabId(const FString& Id)
{
	CurrentPlayerEntityId = Id;
}

void UBasicMatchmakingSystem::CancelMatchmaking()
{
	if (CurrentPlayerEntityId.IsEmpty()) { return; }

	UE_LOG(LogTemp, Log, TEXT("Clearing matchmaking tickets for player %s"), *CurrentPlayerEntityId);
	PlayFab::MultiplayerModels::FCancelAllMatchmakingTicketsForPlayerRequest Request;
	Request.Entity = MakeShared<PlayFab::MultiplayerModels::FEntityKey>(MakePlayerEntity());
	Request.QueueName = QueueMode;

	PlayFabMultiplayerAPI->CancelAllMatchmakingTicketsForPlayer(Request);
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
		if (Result.Status == "Canceled")
		{
			CancelMatchmaking();
		}
		else if (!(Result.MatchId.IsEmpty()))
		{
			HandleMatchFound(Result.MatchId);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Polling GetMatchmakingTicket, current status: %s. %d members currently"), *Result.Status, Result.Members.Num());
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

	CancelMatchmaking();
}

void UBasicMatchmakingSystem::HandleMatchFound(FString MatchId)
{
	PlayFab::MultiplayerModels::FGetMatchRequest Request;
	Request.MatchId = MatchId;
	Request.QueueName = QueueMode;

	auto Success = PlayFab::UPlayFabMultiplayerInstanceAPI::FGetMatchDelegate::CreateLambda(
		[&](const PlayFab::MultiplayerModels::FGetMatchResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("GetMatch %s succeeded! Broadcasting result"), *Result.MatchId);
		bLookingForMatch = false;
		GetWorld()->GetTimerManager().ClearTimer(PollingTimer);
		
		FBasicMatchmakingResult BasicResult = FBasicMatchmakingResult(Result.pfServerDetails);
		
		FString Url = BasicResult.IpAddress + ":" + BasicResult.Port + "?PlayFabId=" + CurrentPlayerEntityId;
		BasicResult.Url = Url;
		MatchResultData = BasicResult;

		OnMatchFound.Broadcast(BasicResult);

		TravelToMatch();
	});

	UE_LOG(LogTemp, Log, TEXT("GetMatch: %s"), *Request.toJSONString());

	PlayFabMultiplayerAPI->GetMatch(Request, Success, ErrorDelegate);
}

void UBasicMatchmakingSystem::TravelToMatch()
{
	FTimerHandle StartMatchTimer;
	GetWorld()->GetTimerManager().SetTimer(StartMatchTimer, [&]() {
		UE_LOG(LogTemp, Log, TEXT("Traveling to match at URL %s"), *MatchResultData.Url);
		
		GetWorld()->GetFirstPlayerController()->ClientTravel(MatchResultData.Url, ETravelType::TRAVEL_Absolute, false);
	}, false, MatchStartDelay);
}

TSharedPtr<FJsonObject> UBasicMatchmakingSystem::MakeMatchmakingAttributes()
{
	FJsonObject AttributesObject;
	FJsonObject DataObject;

	TArray<TSharedPtr<FJsonValue>> Latencies;
	TSharedPtr<FJsonObject> LatencyObject = MakeShared<FJsonObject>();

	// According to this, this is necessary? https://docs.microsoft.com/en-us/gaming/playfab/features/multiplayer/matchmaking/ticket-attributes
	// TODO: Actually perform measurements, however optimized multi-region isn't something that's really "in scope" at the moment
	// Would generally rather set region based on preference
	LatencyObject->SetStringField("region", "EastUs");
	LatencyObject->SetNumberField("latency", 100);
	
	Latencies.Add(MakeShared<FJsonValueObject>(FJsonValueObject(LatencyObject)));

	DataObject.SetArrayField("Latencies", Latencies);

	AttributesObject.SetObjectField("DataObject", MakeShared<FJsonObject>(DataObject));

	return MakeShared<FJsonObject>(AttributesObject);
}

PlayFab::MultiplayerModels::FEntityKey UBasicMatchmakingSystem::MakePlayerEntity()
{
	PlayFab::MultiplayerModels::FEntityKey Entity;

	// TODO: Get this via auth context somehow independent of login method
	Entity.Id = CurrentPlayerEntityId;
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
