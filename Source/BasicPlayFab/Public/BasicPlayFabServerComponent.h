// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BasicPlayFabServerComponent.generated.h"

namespace PlayFab
{
	typedef TSharedPtr<class UPlayFabServerAPI> PlayFabServerPtr;

	namespace ServerModels
	{
		struct FRedeemMatchmakerTicketResult;
		struct FRegisterGameResponse;
		struct FDeregisterGameResponse;
	}

	struct FPlayFabCppError;
}

/**
 * Adds the ability to register a server with playfab
 * Not intended to handle all possible use cases in favor of a simple API
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), Within=GameModeBase)
class BASICPLAYFAB_API UBasicPlayFabServerComponent: public UActorComponent
{
	GENERATED_BODY()

	UBasicPlayFabServerComponent();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=BasicPlayFab)
	FString Build = "default";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = BasicPlayFab)
	FString Mode = "default";

	FString LobbyId;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = BasicPlayFab)
	FString Port = "7777";

	// Get this somehow?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = BasicPlayFab)
	FString Ip = "127.0.0.1";

	PlayFab::PlayFabServerPtr PlayFabServerAPI;

	// Interval in seconds
	float HeartbeatInterval = 60.f;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:
	void Register();
	void Deregister();

	/**
	 * This is meant to be called from GameMode's PreLogin method
	 */
	void ProcessLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

};
