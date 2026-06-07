// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasHousesToVisitxrxs.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTDecorator_HasHousesToVisitxrxs : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_HasHousesToVisitxrxs();

	virtual bool CalculateRawConditionValue(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
