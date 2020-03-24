// MIT License, Nick Arthur github.com/narthur157

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasicPlayFabPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BASICPLAYFAB_API ABasicPlayFabPlayerController : public APlayerController
{
	GENERATED_BODY()
	
		
protected:
	void BeginPlay() override;

	
};
