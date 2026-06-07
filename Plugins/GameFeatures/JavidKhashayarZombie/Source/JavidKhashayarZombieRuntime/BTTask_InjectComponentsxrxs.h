// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_InjectComponentsxrxs.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_InjectComponentsxrxs : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_InjectComponentsxrxs();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
