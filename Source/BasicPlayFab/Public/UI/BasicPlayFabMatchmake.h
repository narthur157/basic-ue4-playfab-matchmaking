// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasicPlayFabMatchmake.generated.h"

class UBasicMatchmakingSystem;

/**
 *
 */
UCLASS()
class BASICPLAYFAB_API UBasicPlayFabMatchmake : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "BasicPlayFab|Matchmaking")
	void FindMatch();

	UFUNCTION(BlueprintPure, Category = "BasicPlayFab|Matchmaking")
	UBasicMatchmakingSystem* GetMatchmakingSystem();

	UPROPERTY(BlueprintReadOnly, Category = "BasicPlayFab|Matchmaking")
	float TimeMatchmakingStarted = -1.f;

	UFUNCTION()
	void HandleMatchFound(FBasicMatchmakingResult ServerDetails);

	UFUNCTION()
	void HandleFailure(const FString& Reason);

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchFound(FBasicMatchmakingResult ServerDetails);

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchmakingFailure(const FString& Reason);
};
