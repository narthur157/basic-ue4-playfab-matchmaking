// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BasicPlayFabServerComponent.generated.h"

class AController;

/**
 * Handles players joining/leaving the game and updating the GSDK accordingly
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), Within=GameModeBase)
class BASICPLAYFAB_API UBasicPlayFabServerComponent: public UActorComponent
{
	GENERATED_BODY()

	UBasicPlayFabServerComponent();

public:
	/**
	 * This is meant to be called from GameMode's PreLogin method
	 */
	void ProcessLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);
	void ProcessLogout(AController* Exiting);
protected:
	void UpdatePlayers();
	TArray<FString> ConnectedPlayerIds;
};
