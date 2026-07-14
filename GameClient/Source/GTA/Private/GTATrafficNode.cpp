#include "GTATrafficNode.h"
#include "DrawDebugHelpers.h"

AGTATrafficNode::AGTATrafficNode()
{
	PrimaryActorTick.bCanEverTick = true;
#if WITH_EDITOR
	PrimaryActorTick.bStartWithTickEnabled = true;
#else
	PrimaryActorTick.bStartWithTickEnabled = false;
#endif

	SpeedLimit = 50.0f; 
}

void AGTATrafficNode::BeginPlay()
{
	Super::BeginPlay();
	
#if !WITH_EDITOR
	SetActorTickEnabled(false);
#endif
}

#if WITH_EDITOR
void AGTATrafficNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (AGTATrafficNode* NextNode : NextNodes)
	{
		if (NextNode)
		{
			DrawDebugDirectionalArrow(
				GetWorld(),
				GetActorLocation(),
				NextNode->GetActorLocation(),
				100.f, 
				FColor::Green,
				false, 
				-1.f, 
				0, 
				5.f 
			);
		}
	}
}
#endif
