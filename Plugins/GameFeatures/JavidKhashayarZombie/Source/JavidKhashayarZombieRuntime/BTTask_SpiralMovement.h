// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpiralMovement.generated.h"

UCLASS()
class JAVIDKHASHAYARZOMBIERUNTIME_API UBTTask_SpiralMovement : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_SpiralMovement();

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector IsExploringKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpiralRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpiralRadiusIncrement = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpiralAngularVelocity = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 600.0f;
};
