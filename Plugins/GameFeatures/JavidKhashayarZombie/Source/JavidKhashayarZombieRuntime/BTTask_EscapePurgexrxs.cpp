// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_EscapePurgexrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "PurgeEscaperxrxs.h"
#include "SpiralMovementxrxs.h"

UBTTask_EscapePurgexrxs::UBTTask_EscapePurgexrxs()
{
	NodeName = "Escape Purge";
	bNotifyTick = true;
}

void UBTTask_EscapePurgexrxs::EndEscape(UBehaviorTreeComponent& OwnerComp) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return;

	if (UPurgeEscaperxrxs* Escaper = Pawn->FindComponentByClass<UPurgeEscaperxrxs>())
		Escaper->StopEscaping();

	if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
		SpiralComp->SetMovementPaused(false);
}

EBTNodeResult::Type UBTTask_EscapePurgexrxs::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn || !Pawn->FindComponentByClass<UPurgeEscaperxrxs>())
		return EBTNodeResult::Failed;

	if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
		SpiralComp->SetMovementPaused(true);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("EscapePurge: getting out!"));

	return EBTNodeResult::InProgress;
}

void UBTTask_EscapePurgexrxs::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UPurgeEscaperxrxs* Escaper = Pawn ? Pawn->FindComponentByClass<UPurgeEscaperxrxs>() : nullptr;
	if (!Escaper)
	{
		EndEscape(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!Escaper->IsInPurgeZone())
	{
		EndEscape(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	Escaper->EscapePurge(DeltaSeconds);
}

EBTNodeResult::Type UBTTask_EscapePurgexrxs::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EndEscape(OwnerComp);
	return EBTNodeResult::Aborted;
}
