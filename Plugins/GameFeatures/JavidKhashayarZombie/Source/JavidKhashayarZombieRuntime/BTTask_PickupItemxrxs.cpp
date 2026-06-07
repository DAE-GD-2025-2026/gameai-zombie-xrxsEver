// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_PickupItemxrxs.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "ItemHandlerxrxs.h"
#include "SpiralMovementxrxs.h"
#include "Items/BaseItem.h"

UBTTask_PickupItemxrxs::UBTTask_PickupItemxrxs()
{
	NodeName = "Pickup Item";
	bNotifyTick = true;
}

void UBTTask_PickupItemxrxs::ResumeMovement(UBehaviorTreeComponent& OwnerComp) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (USpiralMovementxrxs* SpiralComp = Pawn ? Pawn->FindComponentByClass<USpiralMovementxrxs>() : nullptr)
	{
		SpiralComp->SetMovementPaused(false);
	}
}

EBTNodeResult::Type UBTTask_PickupItemxrxs::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn)
		return EBTNodeResult::Failed;

	UItemHandlerxrxs* ItemHandler = Pawn->FindComponentByClass<UItemHandlerxrxs>();
	if (!ItemHandler)
		return EBTNodeResult::Failed;

	ABaseItem* Item = ItemHandler->GetBestItemToCollect();
	if (!Item)
		return EBTNodeResult::Failed;

	FBTPickupItemMemoryxrxs* Memory = new (NodeMemory) FBTPickupItemMemoryxrxs();
	Memory->Target = Item;
	Memory->TimeMoving = 0.0f;
	Memory->Retries = 0;

	// Take movement away from the spiral component while we go grab this
	if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
	{
		SpiralComp->SetMovementPaused(true);
	}

	// Try once right away in case we're already on top of it, then path to it
	if (ItemHandler->TryCollect(Item))
	{
		if (USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>())
			SpiralComp->SetMovementPaused(false);
		return EBTNodeResult::Succeeded;
	}

	// Issue the move ONCE - re-issuing every tick restarts the path and stalls the pawn
	AIController->MoveToActor(Item, 60.0f);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
		FString::Printf(TEXT("PickupItem: going for %s"), *Item->GetName()));

	return EBTNodeResult::InProgress;
}

void UBTTask_PickupItemxrxs::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTPickupItemMemoryxrxs* Memory = reinterpret_cast<FBTPickupItemMemoryxrxs*>(NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn)
	{
		ResumeMovement(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UItemHandlerxrxs* ItemHandler = Pawn->FindComponentByClass<UItemHandlerxrxs>();
	ABaseItem* Item = Memory->Target.Get();

	// Item gone (grabbed by us already, destroyed, or respawned away)
	if (!ItemHandler || !Item || Item->IsHidden())
	{
		ResumeMovement(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// Grab the instant we're in range - don't wait for the move to finish
	if (ItemHandler->TryCollect(Item))
	{
		ResumeMovement(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// The move ended but we still couldn't grab it: retry once, then bail fast (no lingering)
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		if (Memory->Retries >= 1)
		{
			ResumeMovement(OwnerComp);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
		++Memory->Retries;
		AIController->MoveToActor(Item, 60.0f);
	}

	Memory->TimeMoving += DeltaSeconds;
	if (Memory->TimeMoving >= ReachTimeout)
	{
		ResumeMovement(OwnerComp);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
}

EBTNodeResult::Type UBTTask_PickupItemxrxs::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ResumeMovement(OwnerComp);
	return EBTNodeResult::Aborted;
}
