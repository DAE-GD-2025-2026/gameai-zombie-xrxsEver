// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasThreat.generated.h"

struct FHasThreatDecoratorMemory
{
	bool bWasMet = false;
	bool bInitialized = false;
};

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTDecorator_HasThreat : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_HasThreat();

	virtual bool CalculateRawConditionValue(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FHasThreatDecoratorMemory); }
};
