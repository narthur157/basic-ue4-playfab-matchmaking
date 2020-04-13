// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SharedPointer.h"
#include "BasicPlayFabLibrary.generated.h"

class FJsonObject;

/**
 * 
 */
UCLASS()
class BASICPLAYFAB_API UBasicPlayFabLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = BasicPlayFab)
	static bool IsLoggedIn();

	static FString JsonToString(TSharedPtr<FJsonObject> Json);
};
