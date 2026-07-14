#include "GTAMissionManager.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

AGTAMissionManager::AGTAMissionManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGTAMissionManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGTAMissionManager::StartMission(FGTAMissionData MissionData)
{
	ActiveMission = MissionData;
	ActiveMission.Status = EGTAMissionStatus::Active;
	
	UE_LOG(LogTemp, Log, TEXT("Mission Started: %s"), *ActiveMission.MissionName);
	OnMissionStatusChanged.Broadcast(ActiveMission);
}

void AGTAMissionManager::CompleteCurrentObjective(int32 ObjectiveIndex)
{
	if (ActiveMission.Status != EGTAMissionStatus::Active) return;

	if (ActiveMission.Objectives.IsValidIndex(ObjectiveIndex))
	{
		ActiveMission.Objectives[ObjectiveIndex].bIsCompleted = true;
		UE_LOG(LogTemp, Log, TEXT("Objective Completed: %s"), *ActiveMission.Objectives[ObjectiveIndex].Description);

		bool bAllDone = true;
		for (const FGTAMissionObjective& Objective : ActiveMission.Objectives)
		{
			if (!Objective.bIsCompleted)
			{
				bAllDone = false;
				break;
			}
		}

		if (bAllDone)
		{
			CompleteMission();
		}
		else
		{
			OnMissionStatusChanged.Broadcast(ActiveMission);
		}
	}
}

void AGTAMissionManager::CompleteMission()
{
	if (ActiveMission.Status != EGTAMissionStatus::Active) return;

	ActiveMission.Status = EGTAMissionStatus::Completed;
	UE_LOG(LogTemp, Log, TEXT("Mission Completed: %s! Reward: $%d"), *ActiveMission.MissionName, ActiveMission.CashReward);
	
	OnMissionStatusChanged.Broadcast(ActiveMission);
	SaveMissionProgressToServer();
}

void AGTAMissionManager::FailMission()
{
	if (ActiveMission.Status != EGTAMissionStatus::Active) return;

	ActiveMission.Status = EGTAMissionStatus::Failed;
	UE_LOG(LogTemp, Warning, TEXT("Mission Failed: %s"), *ActiveMission.MissionName);

	OnMissionStatusChanged.Broadcast(ActiveMission);
}

void AGTAMissionManager::SaveMissionProgressToServer()
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(TEXT("http://localhost:3001/save/1")); 
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString RequestBody = FString::Printf(
		TEXT("{\"money\": %d, \"completed_mission\": \"%s\"}"), 
		ActiveMission.CashReward, 
		*ActiveMission.MissionID
	);
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Server saved mission progress. Status code: %d"), Res->GetResponseCode());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to connect to save-state server."));
		}
	});

	Request->ProcessRequest();
}
