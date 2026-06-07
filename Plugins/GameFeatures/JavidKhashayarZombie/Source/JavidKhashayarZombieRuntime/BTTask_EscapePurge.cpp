// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_EscapePurge.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "PurgeEscaper.h"
#include "SpiralMovement.h"

UBTTask_EscapePurge::UBTTask_EscapePurge()
{
	NodeName = "Escape Purge";
	bNotifyTick = true;
}

void UBTTask_EscapePurge::EndEscape(UBehaviorTreeComponent& OwnerComp) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return;

	if (UPurgeEscaper* Escaper = Pawn->FindComponentByClass<UPurgeEscaper>())
		Escaper->StopEscaping();

	if (USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>())
		SpiralComp->SetMovementPaused(false);
}

EBTNodeResult::Type UBTTask_EscapePurge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn || !Pawn->FindComponentByClass<UPurgeEscaper>())
		return EBTNodeResult::Failed;

	if (USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>())
		SpiralComp->SetMovementPaused(true);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("EscapePurge: getting out!"));

	return EBTNodeResult::InProgress;
}

void UBTTask_EscapePurge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UPurgeEscaper* Escaper = Pawn ? Pawn->FindComponentByClass<UPurgeEscaper>() : nullptr;
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

EBTNodeResult::Type UBTTask_EscapePurge::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EndEscape(OwnerComp);
	return EBTNodeResult::Aborted;
}
