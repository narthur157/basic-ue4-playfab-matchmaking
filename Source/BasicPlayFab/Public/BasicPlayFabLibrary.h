// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BasicPlayFabLibrary.generated.h"

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
};
