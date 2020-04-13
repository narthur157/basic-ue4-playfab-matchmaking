// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Core/PlayFabClientAPI.h"
#include "PlayFabError.h"
#include "BasicMatchmakingSystem.generated.h"

namespace PlayFab
{
	typedef TSharedPtr<class UPlayFabClientAPI> PlayFabClientPtr;
	typedef TSharedPtr<class UPlayFabMultiplayerAPI> PlayFabMultiplayerPtr;
	typedef TSharedPtr<class UPlayFabMatchmakerAPI> PlayFabMatchmakerPtr;
	typedef TSharedPtr<class UPlayFabAuthenticationAPI> PlayFabAuthenticationPtr;

	namespace ClientModels
	{
		enum Region;
	}

	namespace MultiplayerModels
	{
		struct FServerDetails;
		struct FEntityKey;
	}
}

/**
 * This is the result of matchmaking
 */
USTRUCT(BlueprintType)
struct FBasicMatchmakingResult
{
	GENERATED_BODY()

public:

	FBasicMatchmakingResult() {};

	// Initialize using the PlayFab struct to abstract away the TSharedPtr details
	FBasicMatchmakingResult(TSharedPtr<PlayFab::MultiplayerModels::FServerDetails> PlayfabServerDetails);
	FBasicMatchmakingResult(TSharedPtr<PlayFab::MultiplayerModels::FServerDetails> PlayfabServerDetails, FString Url);

	/**
	 * This is the url to use to travel to the server
	 */
	UPROPERTY(BlueprintReadOnly)
	FString Url;

	UPROPERTY(BlueprintReadOnly)
	FString IpAddress;

	UPROPERTY(BlueprintReadOnly)
	FString Port;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFound, FBasicMatchmakingResult, ServerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchmakingError, const FString&, Reason);

/**
 * Provides a single point of responsibility for matchmaking
 * This is a game instance system so that matchmaking can occur across different maps, and prevent 
 * odd states from occuring, eg if a map travel occurs mid matchmaking
 */
UCLASS()
class BASICPLAYFAB_API UBasicMatchmakingSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UBasicMatchmakingSystem(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * Match data has been loaded, we have an IP + port to connect to
	 */
	UPROPERTY(BlueprintAssignable, Category = "BasicMatchmakingSystem")
	FOnMatchFound OnMatchFound;

	/**
	 * Called if some error occurs at any point in the process
	 */
	UPROPERTY(BlueprintAssignable, Category = "BasicMatchmakingSystem")
	FOnMatchmakingError OnMatchmakingError;
	
	/**
	 * Start looking for a match. Will return false if a search is already in progress
	 * TODO: This should accept a param for which queue to search
	 */
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab|Matchmaking")
	bool FindMatch();

	/**
	 * We really should be able to just call some playfab function to get this
	 * I just can't find it
	 */
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab")
	void SetActivePlayFabId(const FString& Id);


protected:

	// These should be configurable/possibly retrieved from some game mode metadata
	FString QueueMode = "test";
	
	PlayFab::ClientModels::Region Region;
	FString CurrentPlayFabId;

	PlayFab::PlayFabClientPtr PlayFabClientAPI;
	PlayFab::PlayFabMultiplayerPtr PlayFabMultiplayerAPI;
	PlayFab::PlayFabMatchmakerPtr PlayFabMatchmakerAPI;
	PlayFab::PlayFabMatchmakerPtr PlayFabAuthenticationAPI;

	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	bool bLookingForMatch = false;

	FString CurrentTicket;
	FTimerHandle PollingTimer;

	UFUNCTION()
	void PollMatchmakingTicket();

	void HandleMatchmakingFailure(const PlayFab::FPlayFabCppError& Error);

	void HandleMatchFound(FString MatchId);

	TSharedPtr<FJsonObject> MakeMatchmakingAttributes();

	PlayFab::MultiplayerModels::FEntityKey MakePlayerEntity();
};
