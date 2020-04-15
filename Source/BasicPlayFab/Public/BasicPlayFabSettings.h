#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BasicPlayFabSettings.generated.h"

/**
 * Holder for various PlayFab settings
 */
UCLASS(Config=Settings)
class BASICPLAYFAB_API UBasicPlayFabSettings : public UObject
{
	GENERATED_BODY()

public:
	/*UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "BasicPlayFab")
	FString TitleId;

	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "BasicPlayFab")
	FString DeveloperSecretKey;*/

	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "Matchmaking")
	float MatchmakingTimeout = 120.f;
	
	/**
	 * If true, shuts down server after MaxGameLength
	 */
	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "Server")
	bool bUseMaxGameLength = true;

	/**
	 * Maximum amount of time the game session will last before the server is shut down
	 */
	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "Server")
	float MaxGameLength = 300.f;


	static UBasicPlayFabSettings* Get();
};