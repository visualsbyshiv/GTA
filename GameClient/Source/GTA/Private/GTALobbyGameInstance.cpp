#include "GTALobbyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

UGTALobbyGameInstance::UGTALobbyGameInstance()
{
}

void UGTALobbyGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		UE_LOG(LogTemp, Log, TEXT("Found Online Subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGTALobbyGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGTALobbyGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGTALobbyGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGTALobbyGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Online Subsystem found."));
	}
}

void UGTALobbyGameInstance::HostSession(FName SessionName, int32 MaxPlayers)
{
	if (!SessionInterface.IsValid()) return;

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
	if (ExistingSession)
	{
		UE_LOG(LogTemp, Log, TEXT("Destroying existing session before hosting new one..."));
		SessionInterface->DestroySession(SessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true; 
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;

	SessionSettings.Set(SETTING_MAPNAME, FString("OpenWorldMap"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPresence);

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UGTALobbyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session Created Successfully!"));
		UGameplayStatics::OpenLevel(GetWorld(), FName("OpenWorldMap"), true, TEXT("listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session."));
	}
}

void UGTALobbyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("Session Destroyed. Hosting new session..."));
	HostSession(SessionName, 16); 
}

void UGTALobbyGameInstance::FindLobbies()
{
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UGTALobbyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	TArray<FString> LobbiesList;

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Found %d Matchmaking Lobbies!"), SessionSearch->SearchResults.Num());

		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
		{
			const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[i];
			FString HostName = Result.Session.OwningUserName;
			FString MapName;
			Result.Session.SessionSettings.Get(SETTING_MAPNAME, MapName);

			FString Info = FString::Printf(TEXT("%s - Map: %s (%d/%d Players)"), 
				*HostName, 
				*MapName, 
				Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections,
				Result.Session.SessionSettings.NumPublicConnections
			);
			LobbiesList.Add(Info);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Search failed or no sessions found."));
	}

	OnLobbySearchCompleted.Broadcast(LobbiesList);
}

void UGTALobbyGameInstance::JoinLobby(int32 SessionIndex)
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid()) return;

	if (SessionSearch->SearchResults.IsValidIndex(SessionIndex))
	{
		SessionInterface->JoinSession(0, FName("MySession"), SessionSearch->SearchResults[SessionIndex]);
	}
}

void UGTALobbyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		APlayerController* PC = GetFirstLocalPlayerController();
		if (PC)
		{
			FString ConnectionAddress;
			if (SessionInterface->GetResolvedConnectString(SessionName, ConnectionAddress))
			{
				UE_LOG(LogTemp, Log, TEXT("Connecting to server address: %s"), *ConnectionAddress);
				PC->ClientTravel(ConnectionAddress, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to join session."));
	}
}
