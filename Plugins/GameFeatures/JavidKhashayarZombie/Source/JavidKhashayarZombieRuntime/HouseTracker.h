// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HouseTracker.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UHouseTracker : public UActorComponent
{
	GENERATED_BODY()

public:
	UHouseTracker();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddHouseToVisit(AActor* House);
	AActor* GetNextHouseTarget();
	bool HasHousesToVisit() const;
	void MarkHouseVisited(AActor* House);
	void ResetVisitedHouses();

	UPROPERTY(BlueprintReadOnly, Category = "House Tracking")
	TArray<AActor*> HousesToVisit;

	UPROPERTY(BlueprintReadOnly, Category = "House Tracking")
	TArray<AActor*> VisitedHouses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Tracking")
	float HouseVisitRadius = 250.0f;

	// Added for multi-point exploration
	UFUNCTION(BlueprintCallable, Category = "House Tracking")
	TArray<FVector> GenerateExplorationPoints(AActor* House);
};
