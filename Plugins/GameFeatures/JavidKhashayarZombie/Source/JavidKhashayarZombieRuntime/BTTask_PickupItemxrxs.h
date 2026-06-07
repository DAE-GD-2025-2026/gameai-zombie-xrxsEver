// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PickupItemxrxs.generated.h"

class ABaseItem;

struct FBTPickupItemMemoryxrxs
{
	TWeakObjectPtr<ABaseItem> Target;
	float TimeMoving = 0.0f;
	int32 Retries = 0;
};

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_PickupItemxrxs : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PickupItemxrxs();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTPickupItemMemoryxrxs); }

	// Give up on an item we can't reach (kept short so we never linger)
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float ReachTimeout = 4.0f;

private:
	void ResumeMovement(class UBehaviorTreeComponent& OwnerComp) const;
};
