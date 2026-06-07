// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExploreHousexrxs.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_ExploreHousexrxs : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExploreHousexrxs();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
