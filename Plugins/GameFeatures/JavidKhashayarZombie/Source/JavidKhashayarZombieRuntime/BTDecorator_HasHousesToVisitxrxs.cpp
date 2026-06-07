// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasHousesToVisitxrxs.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "HouseTrackerxrxs.h"

UBTDecorator_HasHousesToVisitxrxs::UBTDecorator_HasHousesToVisitxrxs()
{
	NodeName = "Has Houses To Visit?";
}

bool UBTDecorator_HasHousesToVisitxrxs::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UHouseTrackerxrxs* HouseTracker = Pawn->FindComponentByClass<UHouseTrackerxrxs>();
	if (!HouseTracker)
		return false;

	return HouseTracker->HasHousesToVisit();
}
