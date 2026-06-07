// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_InPurgeZone.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "PurgeEscaper.h"

UBTDecorator_InPurgeZone::UBTDecorator_InPurgeZone()
{
	NodeName = "In Purge Zone?";
	bNotifyTick = true;
}

bool UBTDecorator_InPurgeZone::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UPurgeEscaper* Escaper = Pawn->FindComponentByClass<UPurgeEscaper>();
	return Escaper && Escaper->IsInPurgeZone();
}

void UBTDecorator_InPurgeZone::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FInPurgeDecoratorMemory* Memory = reinterpret_cast<FInPurgeDecoratorMemory*>(NodeMemory);
	const bool bMet = CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (!Memory->bInitialized || bMet != Memory->bWasMet)
	{
		Memory->bInitialized = true;
		Memory->bWasMet = bMet;
		OwnerComp.RequestExecution(this);
	}
}
