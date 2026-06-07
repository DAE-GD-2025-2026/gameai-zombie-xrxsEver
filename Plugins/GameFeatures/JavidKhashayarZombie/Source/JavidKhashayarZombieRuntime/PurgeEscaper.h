// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PurgeEscaper.generated.h"

class AAIController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UPurgeEscaper : public UActorComponent
{
	GENERATED_BODY()

public:
	UPurgeEscaper();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsInPurgeZone() const;

	// Driven by the behavior tree task: run clear of the purge zone(s)
	void EscapePurge(float DeltaTime);
	void StopEscaping();

	// React once we're within this distance of a zone's edge (leave before it's too late)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Purge")
	float EnterBuffer = 200.0f;

	// Keep running until we're this far clear of every zone (hysteresis)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Purge")
	float ExitBuffer = 700.0f;

private:
	float GetPurgeRadius(AActor* Zone) const;
	float NearestEdgeDistance() const;
	FVector OutwardDirection() const;
	bool PointIsSafe(const FVector& Point) const;
	FVector FindSafeEscapePoint() const;
	AAIController* GetAI() const;

	bool bEscaping = false;
	FVector LastMoveTarget = FVector(FLT_MAX);
};
