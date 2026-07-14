#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GTALobbyGameInstance.generated.h"

UCLASS()
class GTA_API UGTALobbyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGTALobbyGameInstance();

protected:
	virtual void Init() override;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

public:
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Lobbies")
	void HostSession(FName SessionName, int32 MaxPlayers);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Lobbies")
	void FindLobbies();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Lobbies")
	void JoinLobby(int32 SessionIndex);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbySearchCompleted, const TArray<FString>&, LobbyNames);
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Lobbies")
	FOnLobbySearchCompleted OnLobbySearchCompleted;
};
