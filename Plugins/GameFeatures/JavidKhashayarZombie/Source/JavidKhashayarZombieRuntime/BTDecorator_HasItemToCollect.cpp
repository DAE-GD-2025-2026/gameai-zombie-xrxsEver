// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasItemToCollect.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ItemHandler.h"

UBTDecorator_HasItemToCollect::UBTDecorator_HasItemToCollect()
{
	NodeName = "Has Item To Collect?";
	bNotifyTick = true;
}

bool UBTDecorator_HasItemToCollect::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UItemHandler* ItemHandler = Pawn->FindComponentByClass<UItemHandler>();
	if (!ItemHandler)
		return false;

	return ItemHandler->HasItemToCollect();
}

void UBTDecorator_HasItemToCollect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Re-poll so Observer Aborts can fire while a lower priority task is running
	FHasItemDecoratorMemory* Memory = reinterpret_cast<FHasItemDecoratorMemory*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
