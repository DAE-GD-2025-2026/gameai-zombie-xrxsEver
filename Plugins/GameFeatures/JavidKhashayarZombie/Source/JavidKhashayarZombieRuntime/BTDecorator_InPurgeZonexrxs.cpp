// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_InPurgeZonexrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "PurgeEscaperxrxs.h"

UBTDecorator_InPurgeZonexrxs::UBTDecorator_InPurgeZonexrxs()
{
	NodeName = "In Purge Zone?";
	bNotifyTick = true;
}

bool UBTDecorator_InPurgeZonexrxs::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UPurgeEscaperxrxs* Escaper = Pawn->FindComponentByClass<UPurgeEscaperxrxs>();
	return Escaper && Escaper->IsInPurgeZone();
}

void UBTDecorator_InPurgeZonexrxs::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FInPurgeDecoratorMemoryxrxs* Memory = reinterpret_cast<FInPurgeDecoratorMemoryxrxs*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
