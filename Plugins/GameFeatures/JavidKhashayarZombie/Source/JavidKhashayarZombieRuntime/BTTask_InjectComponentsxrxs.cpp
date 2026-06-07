// Fill out your copyright notice in the description page of Project Settings.

#include "BTTask_InjectComponentsxrxs.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SurvivorComponentInjectorxrxs.h"
#include "SpiralMovementxrxs.h"
#include "EngineUtils.h"

UBTTask_InjectComponentsxrxs::UBTTask_InjectComponentsxrxs()
{
	NodeName = "Inject Survivor Components";
}

EBTNodeResult::Type UBTTask_InjectComponentsxrxs::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("BTTask_InjectComponents: ExecuteTask called!"));

	APawn* Pawn = nullptr;

	// Try method 1: Get pawn from AI Owner
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		Pawn = AIController->GetPawn();
		if (Pawn)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT(" Got pawn from GetAIOwner()->GetPawn()"));
		}
	}

	// Try method 2: Look for pawn in the world (fallback if possession not ready)
	if (!Pawn && OwnerComp.GetWorld())
	{
		for (TActorIterator<APawn> It(OwnerComp.GetWorld()); It; ++It)
		{
			APawn* FoundPawn = *It;
			if (FoundPawn && FoundPawn->GetName().Contains(TEXT("Survivor")))
			{
				Pawn = FoundPawn;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
					FString::Printf(TEXT(" Found survivor pawn in world: %s"), *Pawn->GetName()));
				break;
			}
		}

		if (!Pawn)
		{
			for (TActorIterator<APawn> It(OwnerComp.GetWorld()); It; ++It)
			{
				Pawn = *It;
				if (Pawn)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
						FString::Printf(TEXT(" Found fallback pawn in world: %s"), *Pawn->GetName()));
					break;
				}
			}
		}
	}

	if (!Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BTTask: No pawn found via any method!"));
		return EBTNodeResult::Failed;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
		FString::Printf(TEXT("BTTask: Found pawn: %s"), *Pawn->GetName()));

	// Try to find existing injector
	USurvivorComponentInjectorxrxs* Injector = Pawn->FindComponentByClass<USurvivorComponentInjectorxrxs>();

	if (!Injector)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("BTTask: Creating new injector"));
		// Create injector if it doesn't exist
		Injector = NewObject<USurvivorComponentInjectorxrxs>(Pawn);
		Injector->RegisterComponent();
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("BTTask: Calling InjectComponents..."));

	// Trigger component injection
	Injector->InjectComponents();

	// RE-FIND THE PAWN JUST IN CASE (though it shouldn't have changed)
	// We already have the Pawn pointer.

	// VERIFY SUCCESS
	if (Pawn->FindComponentByClass<USpiralMovementxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT(" Verified: USpiralMovementxrxs found after injection"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT(" ERROR: USpiralMovementxrxs STILL NOT FOUND after injection!"));
		
		// Last ditch effort: call RegisterComponent again or check why it failed
		USpiralMovementxrxs* Spiral = NewObject<USpiralMovementxrxs>(Pawn);
		Spiral->RegisterComponent();
		if (Pawn->FindComponentByClass<USpiralMovementxrxs>())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT(" Fixed: Injected USpiralMovementxrxs via last-ditch effort"));
		}
	}

	// Set the blackboard flag to mark initialization as complete
	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsBool(FName("bIsInitialized"), true);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
			TEXT(" Set bIsInitialized to true in blackboard"));
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BTTask_InjectComponents: SUCCESS"));
	return EBTNodeResult::Succeeded;
}
