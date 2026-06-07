// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasThreat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ThreatHandler.h"

UBTDecorator_HasThreat::UBTDecorator_HasThreat()
{
	NodeName = "Has Threat?";
	bNotifyTick = true;
}

bool UBTDecorator_HasThreat::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UThreatHandler* ThreatHandler = Pawn->FindComponentByClass<UThreatHandler>();
	if (!ThreatHandler)
		return false;

	return ThreatHandler->HasThreat();
}

void UBTDecorator_HasThreat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Re-poll so Observer Aborts can interrupt looting/exploring the moment a zombie shows up
	FHasThreatDecoratorMemory* Memory = reinterpret_cast<FHasThreatDecoratorMemory*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
