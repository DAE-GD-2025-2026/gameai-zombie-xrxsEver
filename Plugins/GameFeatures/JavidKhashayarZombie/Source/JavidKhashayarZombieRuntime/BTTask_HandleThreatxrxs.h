// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HandleThreatxrxs.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_HandleThreatxrxs : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HandleThreatxrxs();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void EndCombat(class UBehaviorTreeComponent& OwnerComp) const;
};
