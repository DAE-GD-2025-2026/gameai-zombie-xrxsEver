// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasThreatxrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ThreatHandlerxrxs.h"

UBTDecorator_HasThreatxrxs::UBTDecorator_HasThreatxrxs()
{
	NodeName = "Has Threat?";
	bNotifyTick = true;
}

bool UBTDecorator_HasThreatxrxs::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UThreatHandlerxrxs* ThreatHandler = Pawn->FindComponentByClass<UThreatHandlerxrxs>();
	if (!ThreatHandler)
		return false;

	return ThreatHandler->HasThreat();
}

void UBTDecorator_HasThreatxrxs::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Re-poll so Observer Aborts can interrupt looting/exploring the moment a zombie shows up
	FHasThreatDecoratorMemoryxrxs* Memory = reinterpret_cast<FHasThreatDecoratorMemoryxrxs*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
