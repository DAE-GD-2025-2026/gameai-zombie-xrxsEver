// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_HasHousesToVisit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "HouseTracker.h"

UBTDecorator_HasHousesToVisit::UBTDecorator_HasHousesToVisit()
{
	NodeName = "Has Houses To Visit?";
}

bool UBTDecorator_HasHousesToVisit::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
		return false;

	UHouseTracker* HouseTracker = Pawn->FindComponentByClass<UHouseTracker>();
	if (!HouseTracker)
		return false;

	return HouseTracker->HasHousesToVisit();
}
