// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_ExploreHousexrxs.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SpiralMovementxrxs.h"
#include "HouseTrackerxrxs.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExploreHousexrxs::UBTTask_ExploreHousexrxs()
{
	NodeName = "Explore House";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ExploreHousexrxs::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>();
	UHouseTrackerxrxs* HouseTracker = Pawn->FindComponentByClass<UHouseTrackerxrxs>();

	if (!SpiralComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BTTask_ExploreHouse: SpiralMovement component not found!"));
		return EBTNodeResult::Failed;
	}

	// Already working a house? Resume where we left off instead of restarting the sweep
	// (a pickup or a zombie may have interrupted us mid-house).
	if (SpiralComp->CurrentState == EMovementStatexrxs::MovingToHouse ||
		SpiralComp->CurrentState == EMovementStatexrxs::ExploringHouse)
	{
		return EBTNodeResult::InProgress;
	}

	if (!HouseTracker || !HouseTracker->HasHousesToVisit())
	{
		return EBTNodeResult::Failed;
	}

	AActor* NextHouse = HouseTracker->GetNextHouseTarget();
	if (!NextHouse)
	{
		return EBTNodeResult::Failed;
	}

	// Set the target in the component
	SpiralComp->MoveToHouse(NextHouse);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, 
		FString::Printf(TEXT("BTTask_ExploreHouse: Moving to house %s"), *NextHouse->GetName()));

	return EBTNodeResult::InProgress;
}

void UBTTask_ExploreHousexrxs::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	USpiralMovementxrxs* SpiralComp = Pawn->FindComponentByClass<USpiralMovementxrxs>();
	if (!SpiralComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Task is finished when component goes back to Spiraling state
	if (SpiralComp->CurrentState == EMovementStatexrxs::Spiraling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BTTask_ExploreHouse: Exploration complete"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
