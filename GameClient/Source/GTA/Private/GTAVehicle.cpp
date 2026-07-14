#include "GTAVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AGTAVehicle::AGTAVehicle()
{
	DriverSeatSocket = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSeatSocket"));
	DriverSeatSocket->SetupAttachment(RootComponent);

	MaxHealth = 200.0f;
	Health = MaxHealth;
	MaxFuel = 100.0f;
	Fuel = MaxFuel;
}

void AGTAVehicle::BeginPlay()
{
	Super::BeginPlay();
}

void AGTAVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ThrottleAction)
		{
			EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AGTAVehicle::ApplyThrottle);
			EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AGTAVehicle::ApplyThrottle);
		}

		if (SteerAction)
		{
			EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Triggered, this, &AGTAVehicle::ApplySteer);
			EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Completed, this, &AGTAVehicle::ApplySteer);
		}

		if (HandbrakeAction)
		{
			EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AGTAVehicle::ApplyHandbrake);
			EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AGTAVehicle::StopHandbrake);
		}
	}
}

void AGTAVehicle::ApplyThrottle(const FInputActionValue& Value)
{
	float ThrottleValue = Value.Get<float>();
	if (UChaosWheeledVehicleMovementComponent* VehicleMovement = GetVehicleMovement())
	{
		VehicleMovement->SetThrottleInput(ThrottleValue);
	}
}

void AGTAVehicle::ApplySteer(const FInputActionValue& Value)
{
	float SteerValue = Value.Get<float>();
	if (UChaosWheeledVehicleMovementComponent* VehicleMovement = GetVehicleMovement())
	{
		VehicleMovement->SetSteeringInput(SteerValue);
	}
}

void AGTAVehicle::ApplyHandbrake(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* VehicleMovement = GetVehicleMovement())
	{
		VehicleMovement->SetHandbrakeInput(true);
	}
}

void AGTAVehicle::StopHandbrake(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* VehicleMovement = GetVehicleMovement())
	{
		VehicleMovement->SetHandbrakeInput(false);
	}
}
