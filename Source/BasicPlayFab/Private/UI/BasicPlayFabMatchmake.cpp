// MIT License, Nick Arthur github.com/narthur157

#include "UI/BasicPlayFabMatchmake.h"
#include "BasicMatchmakingSystem.h"

void UBasicPlayFabMatchmake::FindMatch()
{
	UBasicMatchmakingSystem* Matchmaking = GetGameInstance()->GetSubsystem<UBasicMatchmakingSystem>();

	Matchmaking->OnMatchFound.AddUniqueDynamic(this, &UBasicPlayFabMatchmake::HandleMatchFound);
	Matchmaking->OnMatchmakingError.AddUniqueDynamic(this, &UBasicPlayFabMatchmake::HandleFailure);
	
	Matchmaking->FindMatch();
}

void UBasicPlayFabMatchmake::HandleFailure(const FString& Reason)
{
	OnMatchmakingFailure(Reason);
}

void UBasicPlayFabMatchmake::HandleMatchFound(FBasicMatchmakingResult ServerDetails)
{
	OnMatchFound(ServerDetails);
}


