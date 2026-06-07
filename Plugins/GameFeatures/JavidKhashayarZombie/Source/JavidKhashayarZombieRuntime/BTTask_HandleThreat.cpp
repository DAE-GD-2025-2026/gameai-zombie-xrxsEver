// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_HandleThreat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ThreatHandler.h"
#include "SpiralMovement.h"

UBTTask_HandleThreat::UBTTask_HandleThreat()
{
	NodeName = "Handle Threat";
	bNotifyTick = true;
}

void UBTTask_HandleThreat::EndCombat(UBehaviorTreeComponent& OwnerComp) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return;

	if (UThreatHandler* ThreatHandler = Pawn->FindComponentByClass<UThreatHandler>())
		ThreatHandler->StandDown();

	if (USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>())
		SpiralComp->SetMovementPaused(false);
}

EBTNodeResult::Type UBTTask_HandleThreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn)
		return EBTNodeResult::Failed;

	UThreatHandler* ThreatHandler = Pawn->FindComponentByClass<UThreatHandler>();
	if (!ThreatHandler)
		return EBTNodeResult::Failed;

	// Take movement away from the spiral component while we deal with the zombie
	if (USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>())
		SpiralComp->SetMovementPaused(true);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("HandleThreat: zombie detected!"));

	return EBTNodeResult::InProgress;
}

void UBTTask_HandleThreat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UThreatHandler* ThreatHandler = Pawn ? Pawn->FindComponentByClass<UThreatHandler>() : nullptr;
	if (!ThreatHandler)
	{
		EndCombat(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!ThreatHandler->HasThreat())
	{
		EndCombat(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	ThreatHandler->UpdateCombat(DeltaSeconds);
}

EBTNodeResult::Type UBTTask_HandleThreat::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EndCombat(OwnerComp);
	return EBTNodeResult::Aborted;
}
