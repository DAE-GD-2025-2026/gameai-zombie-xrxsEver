// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SpiralMovement.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SpiralMovement.h"
#include "HouseTracker.h"
#include "EngineUtils.h"

UBTTask_SpiralMovement::UBTTask_SpiralMovement()
{
	NodeName = "Spiral Movement";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SpiralMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = nullptr;

	// Try method 1: Get pawn from AI Owner
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		Pawn = AIController->GetPawn();
	}

	// Try method 2: Look for pawn in the world (fallback)
	if (!Pawn && OwnerComp.GetWorld())
	{
		for (TActorIterator<APawn> It(OwnerComp.GetWorld()); It; ++It)
		{
			APawn* FoundPawn = *It;
			if (FoundPawn && FoundPawn->GetName().Contains(TEXT("Survivor")))
			{
				Pawn = FoundPawn;
				break;
			}
		}

		if (!Pawn)
		{
			for (TActorIterator<APawn> It(OwnerComp.GetWorld()); It; ++It)
			{
				Pawn = *It;
				if (Pawn) break;
			}
		}
	}

	if (!Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SpiralMovement: No pawn found!"));
		return EBTNodeResult::Failed;
	}

	USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>();
	if (!SpiralComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
			FString::Printf(TEXT("SpiralMovement: Component not found on %s! Ensure InjectComponents ran."), *Pawn->GetName()));
		return EBTNodeResult::Failed;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("✓ SpiralMovement task started"));

	// Explicitly set the state to Spiraling as requested
	SpiralComp->CurrentState = EMovementState::Spiraling;

	return EBTNodeResult::InProgress;
}

void UBTTask_SpiralMovement::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Get the USpiralMovement component
	USpiralMovement* SpiralComp = Pawn->FindComponentByClass<USpiralMovement>();
	if (!SpiralComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check if we found houses - if so, finish this task so BT can switch to Explore House
	UHouseTracker* HouseTracker = Pawn->FindComponentByClass<UHouseTracker>();
	if (HouseTracker && HouseTracker->HasHousesToVisit())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("SpiralMovement: House detected, finishing task..."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// Component is ticking on its own - task stays InProgress
}
