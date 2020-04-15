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

	namespace MultiplayerModels
	{
		struct FServerDetails;
		struct FEntityKey;
	}
}

UENUM(BlueprintType)
enum class EMatchmakingStatus : uint8
{
	Inactive,
	WaitingForMatchmakingService,
	WaitingForPlayers,
	WaitingForServer,
	MatchFound,
	InMatch
};


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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchmakingStatusChanged, EMatchmakingStatus, MatchmakingStatus);

/**
 * Provides a single point of responsibility for matchmaking
 * This is a game instance system so that matchmaking can occur across different maps, and prevent 
 * odd states from occuring, eg if a map travel occurs mid matchmaking
 * 
 * FindMatch is the main entry point to matchmaking, this creates a ticket which is then polled until a match is found
 * After the match is found, the client is travelled to the found match
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
	UPROPERTY(BlueprintAssignable, Category = "BasicPlayFab|Matchmaking")
		FOnMatchFound OnMatchFound;

	/**
	 * Called if some error occurs at any point in the process
	 */
	UPROPERTY(BlueprintAssignable, Category = "BasicPlayFab|Matchmaking")
	FOnMatchmakingError OnMatchmakingError;

	/**
	 * Corresponds to EMatchmakingStatus states
	 */
	UPROPERTY(BlueprintAssignable, Category = "BasicPlayFab|Matchmaking")
	FOnMatchmakingStatusChanged OnMatchmakingStatusChanged;

	/**
	 * Start looking for a match. Will return false if a search is already in progress
	 * Mode MUST be a valid Matchmaking Queue mode, or the matchmaking will fail
	 */
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab|Matchmaking")
	bool FindMatch(const FString& Mode);

	/**
	 * Resets all matchmaking related states
	 */
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab|Matchmaking")
	void CancelMatchmaking();

	/**
	 * Has matchmaking been started and not yet cancelled or found
	 */
	UFUNCTION(BlueprintPure, Category = "BasicPlayFab|Matchmaking")
	bool IsMatchmakingActive();

	/**
	 * We really should be able to just call some playfab function to get this
	 * I just can't find it
	 */
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab")
	void SetActivePlayFabId(const FString& Id);


	// UGameInstanceSubsystem Interface
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	// ~UGameInstanceSubsystem

protected:
	EMatchmakingStatus MatchmakingStatus = EMatchmakingStatus::Inactive;

	// These should be configurable/possibly retrieved from some game mode metadata
	FString QueueMode = "test";
	
	PlayFab::ClientModels::Region Region;
	FString CurrentPlayerEntityId;

	float MatchStartDelay = 3.f;

	FBasicMatchmakingResult MatchResultData;

	PlayFab::PlayFabClientPtr PlayFabClientAPI;
	PlayFab::PlayFabMultiplayerPtr PlayFabMultiplayerAPI;
	
	// This error delegate is used for all the potential matchmaking errors, until we have a better idea of which errors need more context
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;


	FString CurrentTicket;
	FTimerHandle PollingTimer;

	/**
	 * Set a new status and trigger the on status change delegate if it's a new status
	 */
	void SetMatchmakingStatus(EMatchmakingStatus Status);

	UFUNCTION()
	void PollMatchmakingTicket();

	void HandleMatchmakingFailure(const PlayFab::FPlayFabCppError& Error);
	void HandleMatchFound(FString MatchId);

	/**
	 * Countdown this->MatchStartDelay seconds and then travel to the url in this->MatchStartDelay
	 */
	void TravelToMatch();

	/**
	 * Produce a JSON object for the CreateMatchmakingTicket request
	 */
	TSharedPtr<FJsonObject> MakeMatchmakingAttributes();

	/**
	 * Uses the class's CurrentPlayerEntityId
	 * This should ideally just be automatically fetched + stored in PlayFab state irrespective of login method
	 */
	PlayFab::MultiplayerModels::FEntityKey MakePlayerEntity();
};
