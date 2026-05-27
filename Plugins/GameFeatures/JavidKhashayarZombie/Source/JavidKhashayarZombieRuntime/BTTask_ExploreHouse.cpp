// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_ExploreHouse.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SpiralMovement.h"
#include "HouseTracker.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExploreHouse::UBTTask_ExploreHouse()
{
	NodeName = "Explore House";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ExploreHouse::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>();
	UHouseTracker* HouseTracker = Pawn->FindComponentByClass<UHouseTracker>();

	if (!SpiralComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BTTask_ExploreHouse: SpiralMovement component not found!"));
		return EBTNodeResult::Failed;
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

void UBTTask_ExploreHouse::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>();
	if (!SpiralComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Task is finished when component goes back to Spiraling state
	if (SpiralComp->CurrentState == EMovementState::Spiraling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BTTask_ExploreHouse: Exploration complete"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
