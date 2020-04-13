// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasicPlayFabMatchmake.generated.h"

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

	UFUNCTION()
	void HandleMatchFound(FBasicMatchmakingResult ServerDetails);

	UFUNCTION()
	void HandleFailure(const FString& Reason);

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchFound(FBasicMatchmakingResult ServerDetails);

	UFUNCTION(BlueprintImplementableEvent, Category = "BasicPlayFab|Matchmaking")
	void OnMatchmakingFailure(const FString& Reason);
};
