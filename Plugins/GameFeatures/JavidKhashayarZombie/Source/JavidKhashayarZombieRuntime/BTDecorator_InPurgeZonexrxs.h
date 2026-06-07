// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InPurgeZonexrxs.generated.h"

struct FInPurgeDecoratorMemoryxrxs
{
	bool bWasMet = false;
	bool bInitialized = false;
};

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTDecorator_InPurgeZonexrxs : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_InPurgeZonexrxs();

	virtual bool CalculateRawConditionValue(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FInPurgeDecoratorMemoryxrxs); }
};
