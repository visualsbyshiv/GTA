#include "GTACharacter.h"
#include "GTAPlayerController.h"
#include "GTAVehicle.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"

AGTACharacter::AGTACharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 	
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	MaxHealth = 100.0f;
	Health = MaxHealth;
	MaxArmor = 100.0f;
	Armor = 0.0f;

	bIsDriving = false;
	CurrentVehicle = nullptr;
}

void AGTACharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGTACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AGTAPlayerController* GTAPC = Cast<AGTAPlayerController>(PC))
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				if (GTAPC->MoveAction)
				{
					EnhancedInputComponent->BindAction(GTAPC->MoveAction, ETriggerEvent::Triggered, this, &AGTACharacter::Move);
				}

				if (GTAPC->LookAction)
				{
					EnhancedInputComponent->BindAction(GTAPC->LookAction, ETriggerEvent::Triggered, this, &AGTACharacter::Look);
				}

				if (GTAPC->JumpAction)
				{
					EnhancedInputComponent->BindAction(GTAPC->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
					EnhancedInputComponent->BindAction(GTAPC->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
				}

				if (GTAPC->InteractAction)
				{
					EnhancedInputComponent->BindAction(GTAPC->InteractAction, ETriggerEvent::Started, this, &AGTACharacter::Interact);
				}
			}
		}
	}
}

void AGTACharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr && !bIsDriving)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGTACharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGTACharacter::Interact()
{
	if (bIsDriving)
	{
		ExitVehicle();
	}
	else
	{
		// Search for nearby vehicles using simple sphere overlap checks
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors, AGTAVehicle::StaticClass());

		AActor* ClosestVehicle = nullptr;
		float MinDistance = 999999.0f;

		for (AActor* Actor : OverlappingActors)
		{
			float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestVehicle = Actor;
			}
		}

		// If a vehicle is found within 3 meters, enter it
		if (ClosestVehicle && MinDistance < 300.0f)
		{
			EnterVehicle(ClosestVehicle);
		}
	}
}

void AGTACharacter::EnterVehicle(AActor* Vehicle)
{
	if (!Vehicle || bIsDriving) return;

	CurrentVehicle = Vehicle;
	bIsDriving = true;

	GetMesh()->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToActor(Vehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("DriverSeat"));

	// Switch player controller possession to the vehicle pawn
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (APawn* VehiclePawn = Cast<APawn>(Vehicle))
		{
			PC->Possess(VehiclePawn);
		}
	}
}

void AGTACharacter::ExitVehicle()
{
	if (!bIsDriving || !CurrentVehicle) return;

	AActor* Vehicle = CurrentVehicle;

	// Reset character visibility & collision BEFORE detaching so we don't fall out of bounds
	GetMesh()->SetVisibility(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	DetachFromActor(FAttachmentTransformRules::KeepWorldTransform);

	// Position character 1.5 meters to the left of the vehicle
	FVector ExitPosition = Vehicle->GetActorLocation() + (Vehicle->GetActorRightVector() * -150.0f) + FVector(0.0f, 0.0f, 90.0f);
	SetActorLocation(ExitPosition);

	CurrentVehicle = nullptr;
	bIsDriving = false;

	// Repossess this character controller back to the character
	if (APawn* VehiclePawn = Cast<APawn>(Vehicle))
	{
		if (APlayerController* PC = Cast<APlayerController>(VehiclePawn->GetController()))
		{
			PC->Possess(this);
		}
	}
}
