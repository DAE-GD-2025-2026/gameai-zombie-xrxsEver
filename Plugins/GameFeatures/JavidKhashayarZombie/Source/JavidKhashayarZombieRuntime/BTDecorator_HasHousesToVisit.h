// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasHousesToVisit.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTDecorator_HasHousesToVisit : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_HasHousesToVisit();

	virtual bool CalculateRawConditionValue(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
