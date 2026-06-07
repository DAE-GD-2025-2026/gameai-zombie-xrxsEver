// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasItemToCollectxrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ItemHandlerxrxs.h"

UBTDecorator_HasItemToCollectxrxs::UBTDecorator_HasItemToCollectxrxs()
{
	NodeName = "Has Item To Collect?";
	bNotifyTick = true;
}

bool UBTDecorator_HasItemToCollectxrxs::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UItemHandlerxrxs* ItemHandler = Pawn->FindComponentByClass<UItemHandlerxrxs>();
	if (!ItemHandler)
		return false;

	return ItemHandler->HasItemToCollect();
}

void UBTDecorator_HasItemToCollectxrxs::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Re-poll so Observer Aborts can fire while a lower priority task is running
	FHasItemDecoratorMemoryxrxs* Memory = reinterpret_cast<FHasItemDecoratorMemoryxrxs*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
