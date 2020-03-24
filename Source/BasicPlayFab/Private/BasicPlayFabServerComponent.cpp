#include "BasicPlayFabServerComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "IBasicPlayFab.h"
#include "PlayFab.h"
#include "Kismet/GameplayStatics.h"
#include "PlayFabServerDataModels.h"
#include "Core/PlayFabServerAPI.h"
#include <PlayFabEnums.h>
#include <PlayFabServerModels.h>

UBasicPlayFabServerComponent::UBasicPlayFabServerComponent()
	: PlayFabServerAPI(IPlayFabModuleInterface::Get().GetServerAPI())
{
	PrimaryComponentTick.TickInterval = HeartbeatInterval;
}

void UBasicPlayFabServerComponent::Register()
{
	if (GetNetMode() != NM_DedicatedServer) { return; }

	PlayFab::ServerModels::FRegisterGameRequest RegisterGameRequest;
	RegisterGameRequest.Build = Build;
	RegisterGameRequest.GameMode = Mode;
	RegisterGameRequest.pfRegion = PlayFab::ServerModels::RegionUSEast;
	RegisterGameRequest.ServerIPV4Address = "127.0.0.1";
	RegisterGameRequest.ServerPort = Port;

	auto RegisterFailure = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&](const PlayFab::FPlayFabCppError& Error) 
	{
	});
	auto RegisterSuccess = PlayFab::UPlayFabServerAPI::FRegisterGameDelegate::CreateLambda(
		[&](const PlayFab::ServerModels::FRegisterGameResponse& Response)
	{
		LobbyId = Response.LobbyId;
		UE_LOG(LogTemp, Log, TEXT("Registered"));
	});

	PlayFabServerAPI->RegisterGame(RegisterGameRequest, RegisterSuccess, RegisterFailure);
}

void UBasicPlayFabServerComponent::Deregister()
{
	if (GetNetMode() != NM_DedicatedServer) { return; }

	PlayFab::ServerModels::FDeregisterGameRequest DeregisterGameRequest;
	DeregisterGameRequest.LobbyId = LobbyId;

	auto DeregisterSuccess = PlayFab::UPlayFabServerAPI::FDeregisterGameDelegate::CreateLambda(
		[&](const PlayFab::ServerModels::FDeregisterGameResponse& Response)
	{
		LobbyId = "";
	});

	auto DeregisterFailure = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&](const PlayFab::FPlayFabCppError& Error)
	{
		LobbyId = "";
	});

	PlayFabServerAPI->DeregisterGame(DeregisterGameRequest, DeregisterSuccess, DeregisterFailure);

	PlayFabServerAPI->DeregisterGame(DeregisterGameRequest,
		PlayFab::UPlayFabServerAPI::FDeregisterGameDelegate::CreateUFunction(this, "HandleDeregisterGame"),
		PlayFab::FPlayFabErrorDelegate::CreateUFunction(this, "HandleDeregisterGameFailure"));
}

void UBasicPlayFabServerComponent::ProcessLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (GetNetMode() != NM_DedicatedServer) { return; }

	FString Ticket = UGameplayStatics::ParseOption(Options, "ticket");
	PlayFab::ServerModels::FRedeemMatchmakerTicketRequest TicketRequest;

	TicketRequest.LobbyId = LobbyId;
	TicketRequest.Ticket = Ticket;

	auto SuccessHandler = PlayFab::UPlayFabServerAPI::FRedeemMatchmakerTicketDelegate::CreateLambda(
		[&, Ticket, Options](const PlayFab::ServerModels::FRedeemMatchmakerTicketResult& TicketResult)
	{
		UE_LOG(LogTemp, Log, TEXT("Logged in player with ticket %s"), *Ticket)
		FString Msg = GetOuterAGameModeBase()->GameSession->ApproveLogin(Options);
		FGameModeEvents::GameModePreLoginEvent.Broadcast(GetOuterAGameModeBase(), UniqueId, Msg);
	});

	auto FailureHandler = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&, Ticket](const PlayFab::FPlayFabCppError&) 
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to log in player with ticket %s"), *Ticket)
		//ErrorMessage = TEXT("Failed to validate player with ticket " + Ticket);
	});

	PlayFabServerAPI->RedeemMatchmakerTicket(TicketRequest, SuccessHandler, FailureHandler);

}

void UBasicPlayFabServerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetNetMode() != NM_DedicatedServer) { return; }

	if (LobbyId.IsEmpty()) {
		return;
	}

	PlayFab::ServerModels::FRefreshGameServerInstanceHeartbeatRequest Request;
	Request.LobbyId = LobbyId;

	auto Success = PlayFab::UPlayFabServerAPI::FRefreshGameServerInstanceHeartbeatDelegate::CreateLambda(
		[&](const PlayFab::ServerModels::FRefreshGameServerInstanceHeartbeatResult& Result)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Server heartbeat for lobby %s"), *LobbyId)
	});

	auto FailureHandler = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&](const PlayFab::FPlayFabCppError&)
	{
		//ErrorMessage = TEXT("Failed to validate player");
	});

	PlayFabServerAPI->RefreshGameServerInstanceHeartbeat(Request, Success, FailureHandler);
}
