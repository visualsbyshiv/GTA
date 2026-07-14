#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GTAMissionManager.generated.h"

class AGTAMissionManager;

UENUM(BlueprintType)
enum class EGTAMissionStatus : uint8
{
	NotStarted   UMETA(DisplayName = "Not Started"),
	Active       UMETA(DisplayName = "Active"),
	Completed    UMETA(DisplayName = "Completed"),
	Failed       UMETA(DisplayName = "Failed")
};

USTRUCT(BlueprintType)
struct FGTAMissionObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	bool bIsCompleted;

	FGTAMissionObjective()
		: Description(TEXT(""))
		, bIsCompleted(false)
	{}
};

USTRUCT(BlueprintType)
struct FGTAMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString MissionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FGTAMissionObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	EGTAMissionStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	int32 CashReward;

	FGTAMissionData()
		: MissionID(TEXT(""))
		, MissionName(TEXT(""))
		, Status(EGTAMissionStatus::NotStarted)
		, CashReward(1000)
	{}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStatusChanged, const FGTAMissionData&, MissionData);

UCLASS()
class GTA_API AGTAMissionManager : public AInfo
{
	GENERATED_BODY()

public:
	AGTAMissionManager();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionStatusChanged OnMissionStatusChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	FGTAMissionData ActiveMission;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMission(FGTAMissionData MissionData);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void CompleteCurrentObjective(int32 ObjectiveIndex);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void CompleteMission();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void FailMission();

private:
	void SaveMissionProgressToServer();
};
