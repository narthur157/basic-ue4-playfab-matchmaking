// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasicPlayFabMatchmake.generated.h"

namespace PlayFab
{
	typedef TSharedPtr<class UPlayFabClientAPI> PlayFabClientPtr;
	namespace ClientModels
	{
		enum Region;
	}
}

/**
 * 
 */
UCLASS()
class BASICPLAYFAB_API UBasicPlayFabMatchmake : public UUserWidget
{
	GENERATED_BODY()

	UBasicPlayFabMatchmake(const FObjectInitializer& ObjectInitializer);

protected:
	// These should be configurable/possibly retrieved from some game mode metadata
	FString Build = "default";
	FString Mode = "default";
	PlayFab::ClientModels::Region Region;

	PlayFab::PlayFabClientPtr PlayFabServerAPI;

	UFUNCTION(BlueprintCallable, Category="BasicPlayFab|Matchmaking")
	void FindMatch();

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchFound();

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchmakingFailure();
};
