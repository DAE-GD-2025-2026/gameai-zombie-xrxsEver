// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InPurgeZone.generated.h"

struct FInPurgeDecoratorMemory
{
	bool bWasMet = false;
	bool bInitialized = false;
};

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTDecorator_InPurgeZone : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_InPurgeZone();

	virtual bool CalculateRawConditionValue(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FInPurgeDecoratorMemory); }
};
