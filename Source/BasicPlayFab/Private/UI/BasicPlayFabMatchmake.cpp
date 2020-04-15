// MIT License, Nick Arthur github.com/narthur157

#include "UI/BasicPlayFabMatchmake.h"
#include "BasicMatchmakingSystem.h"

void UBasicPlayFabMatchmake::FindMatch()
{
	UBasicMatchmakingSystem* Matchmaking = GetMatchmakingSystem();

	Matchmaking->OnMatchFound.AddUniqueDynamic(this, &UBasicPlayFabMatchmake::HandleMatchFound);
	Matchmaking->OnMatchmakingError.AddUniqueDynamic(this, &UBasicPlayFabMatchmake::HandleFailure);
	
	Matchmaking->FindMatch("test");
}

UBasicMatchmakingSystem* UBasicPlayFabMatchmake::GetMatchmakingSystem()
{
	return GetGameInstance()->GetSubsystem<UBasicMatchmakingSystem>();
}

void UBasicPlayFabMatchmake::HandleFailure(const FString& Reason)
{
	OnMatchmakingFailure(Reason);
}

void UBasicPlayFabMatchmake::HandleMatchFound(FBasicMatchmakingResult ServerDetails)
{
	OnMatchFound(ServerDetails);
}


