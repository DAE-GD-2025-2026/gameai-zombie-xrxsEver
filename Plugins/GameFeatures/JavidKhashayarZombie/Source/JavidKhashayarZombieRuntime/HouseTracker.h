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

	void DiscoverHouses();
	bool IsHouseActor(AActor* Actor) const;

	UPROPERTY(BlueprintReadOnly, Category = "House Tracking")
	TArray<AActor*> KnownHouses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Tracking")
	FString HouseActorTag = TEXT("House");

	// How many other houses must be visited before a house can be revisited
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Tracking")
	int32 RevisitCooldownVisits = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Tracking")
	float RescanInterval = 3.0f;

	UFUNCTION(BlueprintCallable, Category = "House Tracking")
	TArray<FVector> GenerateExplorationPoints(AActor* House);

private:
	bool IsOnCooldown(AActor* House) const;

	int32 VisitCounter = 0;
	TMap<AActor*, int32> VisitedAtCount;
	float RescanTimer = 0.0f;
};
