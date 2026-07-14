#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GTACharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;

UCLASS()
class GTA_API AGTACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGTACharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxArmor;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<AActor> CurrentVehicle;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bIsDriving;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EnterVehicle(AActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ExitVehicle();
};
