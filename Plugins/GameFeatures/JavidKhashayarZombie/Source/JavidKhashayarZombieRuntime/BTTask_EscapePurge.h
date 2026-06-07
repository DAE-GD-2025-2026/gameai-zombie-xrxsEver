// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EscapePurge.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_EscapePurge : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EscapePurge();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void EndEscape(class UBehaviorTreeComponent& OwnerComp) const;
};
