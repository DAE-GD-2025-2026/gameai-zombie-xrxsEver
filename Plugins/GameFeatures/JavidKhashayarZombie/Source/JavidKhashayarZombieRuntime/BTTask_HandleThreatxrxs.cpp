// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_HandleThreatxrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "ThreatHandlerxrxs.h"
#include "SpiralMovementxrxs.h"

UBTTask_HandleThreatxrxs::UBTTask_HandleThreatxrxs()
{
	NodeName = "Handle Threat";
	bNotifyTick = true;
}

void UBTTask_HandleThreatxrxs::EndCombat(UBehaviorTreeComponent& OwnerComp) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return;

	if (UThreatHandlerxrxs* ThreatHandler = Pawn->FindComponentByClass<UThreatHandlerxrxs>())
		ThreatHandler->StandDown();

	if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
		SpiralComp->SetMovementPaused(false);
}

EBTNodeResult::Type UBTTask_HandleThreatxrxs::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn)
		return EBTNodeResult::Failed;

	UThreatHandlerxrxs* ThreatHandler = Pawn->FindComponentByClass<UThreatHandlerxrxs>();
	if (!ThreatHandler)
		return EBTNodeResult::Failed;

	// Take movement away from the spiral component while we deal with the zombie
	if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
		SpiralComp->SetMovementPaused(true);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("HandleThreat: zombie detected!"));

	return EBTNodeResult::InProgress;
}

void UBTTask_HandleThreatxrxs::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UThreatHandlerxrxs* ThreatHandler = Pawn ? Pawn->FindComponentByClass<UThreatHandlerxrxs>() : nullptr;
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

EBTNodeResult::Type UBTTask_HandleThreatxrxs::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EndCombat(OwnerComp);
	return EBTNodeResult::Aborted;
}
