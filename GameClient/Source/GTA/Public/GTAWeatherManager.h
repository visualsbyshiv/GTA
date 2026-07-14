#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GTAWeatherManager.generated.h"

class ADirectionalLight;
class ASkyLight;

UENUM(BlueprintType)
enum class EGTAWeatherState : uint8
{
	Sunny       UMETA(DisplayName = "Sunny"),
	Cloudy      UMETA(DisplayName = "Cloudy"),
	Rainy       UMETA(DisplayName = "Rainy"),
	Stormy      UMETA(DisplayName = "Stormy")
};

UCLASS()
class GTA_API AGTAWeatherManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGTAWeatherManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float CurrentHour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeMultiplier; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_WeatherState, Category = "Weather")
	EGTAWeatherState WeatherState;

	UFUNCTION()
	void OnRep_WeatherState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights")
	TObjectPtr<ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights")
	TObjectPtr<ASkyLight> SkyLight;

	UPROPERTY(BlueprintReadOnly, Category = "Weather")
	float TargetCloudDensity;

	UPROPERTY(BlueprintReadOnly, Category = "Weather")
	float TargetRainIntensity;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void UpdateTimeOfDay(float DeltaTime);
	void UpdateWeatherInterpolation(float DeltaTime);

	float CurrentCloudDensity;
	float CurrentRainIntensity;
};
