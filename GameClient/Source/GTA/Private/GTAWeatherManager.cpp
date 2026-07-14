#include "GTAWeatherManager.h"
#include "Engine/DirectionalLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/LightComponent.h"
#include "Net/UnrealNetwork.h"

AGTAWeatherManager::AGTAWeatherManager()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	CurrentHour = 12.0f;
	TimeMultiplier = 60.0f;
	WeatherState = EGTAWeatherState::Sunny;

	TargetCloudDensity = 0.1f;
	TargetRainIntensity = 0.0f;
	CurrentCloudDensity = 0.1f;
	CurrentRainIntensity = 0.0f;

	SunLight = nullptr;
	SkyLight = nullptr;
}

void AGTAWeatherManager::BeginPlay()
{
	Super::BeginPlay();
	OnRep_WeatherState(); 
}

void AGTAWeatherManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTimeOfDay(DeltaTime);
	UpdateWeatherInterpolation(DeltaTime);
}

void AGTAWeatherManager::UpdateTimeOfDay(float DeltaTime)
{
	if (!HasAuthority()) return; 

	CurrentHour += (DeltaTime * TimeMultiplier) / 3600.0f;
	if (CurrentHour >= 24.0f)
	{
		CurrentHour -= 24.0f;
	}

	if (SunLight && SunLight->GetLightComponent())
	{
		float PitchRotation = ((CurrentHour - 6.0f) / 24.0f) * 360.0f;
		FRotator NewRotation = FRotator(PitchRotation, -90.0f, 0.0f); 
		SunLight->SetActorRotation(NewRotation);

		if (PitchRotation > 180.0f || PitchRotation < 0.0f)
		{
			SunLight->GetLightComponent()->SetIntensity(0.0f); 
		}
		else
		{
			float IntensityFactor = FMath::Sin(FMath::DegreesToRadians(PitchRotation));
			SunLight->GetLightComponent()->SetIntensity(IntensityFactor * 10.0f);
		}
	}
}

void AGTAWeatherManager::UpdateWeatherInterpolation(float DeltaTime)
{
	CurrentCloudDensity = FMath::FInterpTo(CurrentCloudDensity, TargetCloudDensity, DeltaTime, 0.5f);
	CurrentRainIntensity = FMath::FInterpTo(CurrentRainIntensity, TargetRainIntensity, DeltaTime, 0.5f);
}

void AGTAWeatherManager::OnRep_WeatherState()
{
	switch (WeatherState)
	{
	case EGTAWeatherState::Sunny:
		TargetCloudDensity = 0.1f;
		TargetRainIntensity = 0.0f;
		break;
	case EGTAWeatherState::Cloudy:
		TargetCloudDensity = 0.6f;
		TargetRainIntensity = 0.0f;
		break;
	case EGTAWeatherState::Rainy:
		TargetCloudDensity = 0.85f;
		TargetRainIntensity = 0.6f;
		break;
	case EGTAWeatherState::Stormy:
		TargetCloudDensity = 1.0f;
		TargetRainIntensity = 1.0f;
		break;
	}
}

void AGTAWeatherManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGTAWeatherManager, WeatherState);
}
