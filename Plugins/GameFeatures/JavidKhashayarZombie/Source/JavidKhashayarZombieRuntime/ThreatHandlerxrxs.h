// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemType.h"
#include "ThreatHandlerxrxs.generated.h"

class ABaseZombie;
class AAIController;
class ASurvivorPawn;
class UInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UThreatHandlerxrxs : public UActorComponent
{
	GENERATED_BODY()

public:
	UThreatHandlerxrxs();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool HasThreat() const;
	ABaseZombie* GetNearestThreat() const;

	// Driven by the behavior tree task every tick while a threat is around
	void UpdateCombat(float DeltaTime);
	// Stop running / moving once the threat is gone
	void StandDown();

	// Only stop looting and deal with a zombie once it's this close (a real touch threat)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float EngageEnterRadius = 650.0f;

	// Keep dealing with it until it gets this far away (hysteresis so we don't flip-flop)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float EngageExitRadius = 1000.0f;

	// Pistol is accurate at range; the shotgun only bites up close (it fires a spread of pellets)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float PistolRange = 800.0f;

	// Back away while shooting once a zombie is closer than this (pistol)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float PistolKiteDistance = 400.0f;

	// Let zombies get this close before unloading the shotgun so the pellets connect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float ShotgunRange = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float ShotgunKiteDistance = 250.0f;

	// Only sprint (burns stamina) when a zombie is at least this close
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float RunWhenWithin = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float FireInterval = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float FleeStep = 900.0f;

	// If we can't shoot or get closer for this long, give up on this zombie and
	// let the tree do something useful for a bit instead of freezing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float StuckTimeout = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float IgnoreDurationOnStuck = 4.0f;

	// How far to look for a dropped gun / a house when unarmed and chased
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float ArmUpSearchRadius = 2500.0f;

private:
	ABaseZombie* GetNearestZombie(float Radius) const;
	FVector GetRepulsionDirection() const;
	bool HasLineOfSight(const AActor* Target) const;
	int32 FindWeaponSlotWithAmmo() const;
	int32 SlotOfWeaponWithAmmo(EItemType Type) const;

	void HandleArmed(float DeltaTime, ABaseZombie* Threat);
	void HandleUnarmed(const FVector& DirToThreat, const FVector& Away);

	void FaceInstant(const FVector& Direction);
	void MoveAwayTo(const FVector& Direction);
	void SetRunning(bool bRun);

	// Issue moves without restarting the path every frame (that's what makes it stall)
	void MoveToActorTracked(AActor* Goal, float AcceptanceRadius);
	void MoveToPointTracked(const FVector& Point);
	void HoldPosition();

	AAIController* GetAI() const;
	UInventoryComponent* GetInventory() const;

	float FireTimer = 0.0f;
	bool bEngaged = false;

	// Movement bookkeeping so we only re-issue a move when something actually changed
	TWeakObjectPtr<AActor> MoveGoal;
	float MoveRefreshTimer = 0.0f;
	float CurrentDelta = 0.0f;

	// Stuck detection: bail out of a fight we can't make progress in
	float StuckTime = 0.0f;
	float LastThreatDist = 1.0e9f;
	double IgnoreThreatsUntil = 0.0;
};
