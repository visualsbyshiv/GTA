#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GTATrafficNode.generated.h"

UCLASS()
class GTA_API AGTATrafficNode : public AActor
{
	GENERATED_BODY()
	
public:	
	AGTATrafficNode();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Lane")
	TArray<TObjectPtr<AGTATrafficNode>> NextNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Lane")
	float SpeedLimit;

#if WITH_EDITOR
	virtual void Tick(float DeltaTime) override;
#endif
};
