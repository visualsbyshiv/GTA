#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "GTAVehicle.generated.h"

class USceneComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class GTA_API AGTAVehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AGTAVehicle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Driving inputs
	void ApplyThrottle(const FInputActionValue& Value);
	void ApplySteer(const FInputActionValue& Value);
	void ApplyHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Seats")
	TObjectPtr<USceneComponent> DriverSeatSocket;

	// Enhanced Input bindings for driving mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> VehicleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SteerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> HandbrakeAction;

	// Vehicle Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Fuel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxFuel;
};
