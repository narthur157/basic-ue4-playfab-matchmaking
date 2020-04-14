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

#if UE_SERVER
#include "gsdk.h"
#include "string.h"
#include <vector>
#endif

UBasicPlayFabServerComponent::UBasicPlayFabServerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBasicPlayFabServerComponent::ProcessLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
#if UE_SERVER
	FString PlayerPFId = UGameplayStatics::ParseOption(Options, "PlayFabId");
	ConnectedPlayerIds.Add(PlayerPFId);
	UpdatePlayers();
#endif
}

void UBasicPlayFabServerComponent::ProcessLogout(AController* Exiting)
{
#if UE_SERVER
	//APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	//PlayerState->UniqueId;
#endif
}

void UBasicPlayFabServerComponent::UpdatePlayers()
{
#if UE_SERVER
	std::vector<Microsoft::Azure::Gaming::ConnectedPlayer> GSDKPlayers;

	for (FString Id : ConnectedPlayerIds)
	{
		Microsoft::Azure::Gaming::ConnectedPlayer GSDKPlayer = Microsoft::Azure::Gaming::ConnectedPlayer(TCHAR_TO_UTF8(*Id));
		//GSDKPlayer.m_playerId = TCHAR_TO_UTF8(*PlayerPFId);
		GSDKPlayers.push_back(GSDKPlayer);
	}

	Microsoft::Azure::Gaming::GSDK::updateConnectedPlayers(GSDKPlayers);
#endif
}
