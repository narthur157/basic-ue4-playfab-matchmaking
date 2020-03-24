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

	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "BasicPlayFab")
	FString TitleId;

	UPROPERTY(GlobalConfig, EditDefaultsOnly, Category = "BasicPlayFab")
	FString DeveloperSecretKey;
};
