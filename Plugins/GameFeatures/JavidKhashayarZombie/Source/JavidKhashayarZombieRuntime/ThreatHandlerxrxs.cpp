// Fill out your copyright notice in the Description page of Project Settings.

#include "ThreatHandlerxrxs.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "ItemHandlerxrxs.h"
#include "HouseTrackerxrxs.h"
#include "Zombies/BaseZombie.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UThreatHandlerxrxs::UThreatHandlerxrxs()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UThreatHandlerxrxs::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the engaged state with hysteresis so the BT doesn't flip-flop
	const ABaseZombie* Nearest = GetNearestZombie(EngageExitRadius);
	const float Dist = Nearest && GetOwner()
		? FVector::Dist(GetOwner()->GetActorLocation(), Nearest->GetActorLocation())
		: TNumericLimits<float>::Max();

	const bool bIgnoring = GetWorld() && GetWorld()->GetTimeSeconds() < IgnoreThreatsUntil;

	if (!bEngaged && Dist <= EngageEnterRadius && !bIgnoring)
		bEngaged = true;
	else if (bEngaged && Dist > EngageExitRadius)
		bEngaged = false;
}

AAIController* UThreatHandlerxrxs::GetAI() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
}

UInventoryComponent* UThreatHandlerxrxs::GetInventory() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UInventoryComponent>() : nullptr;
}

ABaseZombie* UThreatHandlerxrxs::GetNearestZombie(float Radius) const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return nullptr;

	const FVector Me = Owner->GetActorLocation();
	const float RadiusSq = Radius * Radius;

	ABaseZombie* Best = nullptr;
	float BestDist = TNumericLimits<float>::Max();

	for (TActorIterator<ABaseZombie> It(World); It; ++It)
	{
		ABaseZombie* Zombie = *It;
		const float Dist = FVector::DistSquared(Me, Zombie->GetActorLocation());
		if (Dist > RadiusSq)
			continue;

		if (Dist < BestDist)
		{
			BestDist = Dist;
			Best = Zombie;
		}
	}

	return Best;
}

ABaseZombie* UThreatHandlerxrxs::GetNearestThreat() const
{
	return GetNearestZombie(EngageExitRadius);
}

bool UThreatHandlerxrxs::HasThreat() const
{
	return bEngaged;
}

FVector UThreatHandlerxrxs::GetRepulsionDirection() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return FVector::ZeroVector;

	const FVector Me = Owner->GetActorLocation();
	const float RadiusSq = EngageExitRadius * EngageExitRadius;

	FVector Sum = FVector::ZeroVector;
	for (TActorIterator<ABaseZombie> It(World); It; ++It)
	{
		FVector Away = Me - It->GetActorLocation();
		Away.Z = 0.f;
		const float Dist = Away.Size();
		if (Dist <= KINDA_SMALL_NUMBER || Dist * Dist > RadiusSq)
			continue;

		// Closer zombies push harder
		Sum += Away / (Dist * Dist);
	}

	return Sum.GetSafeNormal();
}

bool UThreatHandlerxrxs::HasLineOfSight(const AActor* Target) const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !Target || !World)
		return false;

	const FVector Start = Owner->GetActorLocation() + FVector(0, 0, 50.f);
	const FVector End = Target->GetActorLocation() + FVector(0, 0, 50.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	Params.AddIgnoredActor(Target);

	FHitResult Hit;
	const bool bBlocked = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	return !bBlocked;
}

int32 UThreatHandlerxrxs::FindWeaponSlotWithAmmo() const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return -1;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	int32 PistolSlot = -1;
	int32 ShotgunSlot = -1;
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (!Items[i] || Items[i]->GetValue() <= 0)
			continue;

		const EItemType Type = Items[i]->GetItemType();
		if (Type == EItemType::Pistol && PistolSlot < 0)
			PistolSlot = i;
		else if (Type == EItemType::Shotgun && ShotgunSlot < 0)
			ShotgunSlot = i;
	}

	// Prefer the pistol: it's accurate, the shotgun sprays
	return PistolSlot >= 0 ? PistolSlot : ShotgunSlot;
}

void UThreatHandlerxrxs::FaceInstant(const FVector& Direction)
{
	AActor* Owner = GetOwner();
	if (!Owner || Direction.IsNearlyZero())
		return;

	FRotator Rot = Direction.Rotation();
	Rot.Pitch = 0.f;
	Rot.Roll = 0.f;
	Owner->SetActorRotation(Rot);
}

void UThreatHandlerxrxs::MoveAwayTo(const FVector& Direction)
{
	AAIController* AI = GetAI();
	AActor* Owner = GetOwner();
	if (!AI || !Owner)
		return;

	FVector Dir = Direction.GetSafeNormal();
	if (Dir.IsNearlyZero())
		Dir = -Owner->GetActorForwardVector();

	FVector Target = Owner->GetActorLocation() + Dir * FleeStep;
	if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation NavLoc;
		if (Nav->ProjectPointToNavigation(Target, NavLoc, FVector(300.f, 300.f, 500.f)))
			Target = NavLoc.Location;
	}

	MoveToPointTracked(Target);
	DrawDebugLine(GetWorld(), Owner->GetActorLocation(), Target, FColor::Red, false, 0.1f, 0, 2.f);
}

void UThreatHandlerxrxs::MoveToActorTracked(AActor* Goal, float AcceptanceRadius)
{
	AAIController* AI = GetAI();
	if (!AI || !Goal)
		return;

	// MoveToActor already tracks a moving goal, so only (re)issue when the goal
	// changed or path following has stopped - re-issuing every frame stalls the pawn.
	if (MoveGoal.Get() != Goal || AI->GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		AI->MoveToActor(Goal, AcceptanceRadius);
		MoveGoal = Goal;
	}
}

void UThreatHandlerxrxs::MoveToPointTracked(const FVector& Point)
{
	AAIController* AI = GetAI();
	if (!AI)
		return;

	MoveGoal = nullptr;
	MoveRefreshTimer -= CurrentDelta;

	// Flee/kite points drift, so refresh them on a short timer instead of every frame
	if (MoveRefreshTimer <= 0.f || AI->GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		AI->MoveToLocation(Point, 50.f);
		MoveRefreshTimer = 0.25f;
	}
}

void UThreatHandlerxrxs::HoldPosition()
{
	AAIController* AI = GetAI();
	if (!AI)
		return;

	if (AI->GetMoveStatus() == EPathFollowingStatus::Moving)
		AI->StopMovement();

	MoveGoal = nullptr;
}

void UThreatHandlerxrxs::UpdateCombat(float DeltaTime)
{
	CurrentDelta = DeltaTime;

	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	if (!Pawn)
		return;

	ABaseZombie* Threat = GetNearestThreat();
	if (!Threat)
		return;

	const FVector DirToThreat = (Threat->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
	const FVector Away = GetRepulsionDirection();

	if (FindWeaponSlotWithAmmo() >= 0)
	{
		HandleArmed(DeltaTime, Threat);
	}
	else
	{
		HandleUnarmed(DirToThreat, Away);
	}
}

int32 UThreatHandlerxrxs::SlotOfWeaponWithAmmo(EItemType Type) const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return -1;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] && Items[i]->GetItemType() == Type && Items[i]->GetValue() > 0)
			return i;
	}
	return -1;
}

void UThreatHandlerxrxs::HandleArmed(float DeltaTime, ABaseZombie* Threat)
{
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	if (!Pawn)
		return;

	const FVector Me = Pawn->GetActorLocation();
	const float Dist = FVector::Dist(Me, Threat->GetActorLocation());
	const FVector DirToThreat = (Threat->GetActorLocation() - Me).GetSafeNormal();
	const bool bLineOfSight = HasLineOfSight(Threat);

	// Pick the right gun for the range: shotgun shreds up close, pistol reaches further
	const int32 ShotgunSlot = SlotOfWeaponWithAmmo(EItemType::Shotgun);
	const int32 PistolSlot = SlotOfWeaponWithAmmo(EItemType::Pistol);

	int32 WeaponSlot;
	float WeaponRange;
	float KiteDistance;
	if (ShotgunSlot >= 0 && (PistolSlot < 0 || Dist <= ShotgunRange))
	{
		WeaponSlot = ShotgunSlot;
		WeaponRange = ShotgunRange;
		KiteDistance = ShotgunKiteDistance;
	}
	else
	{
		WeaponSlot = PistolSlot;
		WeaponRange = PistolRange;
		KiteDistance = PistolKiteDistance;
	}

	FaceInstant(DirToThreat);

	const bool bCanShoot = bLineOfSight && Dist <= WeaponRange;

	if (bCanShoot)
	{
		FireTimer -= DeltaTime;
		if (FireTimer <= 0.f)
		{
			if (UInventoryComponent* Inv = GetInventory())
				Inv->UseItem(WeaponSlot);
			FireTimer = FireInterval;
		}

		if (Dist < KiteDistance)
		{
			// Too close: kite backwards while still facing/shooting
			SetRunning(true);
			MoveAwayTo(GetRepulsionDirection());
		}
		else
		{
			// In range with a clear shot: hold and fire
			SetRunning(false);
			HoldPosition();
		}
	}
	else
	{
		// Can't shoot yet (wall in the way, or out of this weapon's range): close right in
		// so we get the shot - never stop and wait. Tight acceptance when blind so we round
		// the corner; otherwise get inside the weapon's range.
		SetRunning(Dist < RunWhenWithin);
		const float Accept = bLineOfSight ? FMath::Max(WeaponRange * 0.8f, 120.f) : 120.f;
		MoveToActorTracked(Threat, Accept);
	}

	// Bail out if we can neither shoot nor make any movement progress for a while
	const bool bMoving = GetAI() && GetAI()->GetMoveStatus() == EPathFollowingStatus::Moving;
	const bool bProgress = bCanShoot || bMoving || (Dist < LastThreatDist - 10.f);
	LastThreatDist = Dist;
	StuckTime = bProgress ? 0.f : StuckTime + DeltaTime;
	if (StuckTime >= StuckTimeout)
	{
		StuckTime = 0.f;
		bEngaged = false;
		if (GetWorld())
			IgnoreThreatsUntil = GetWorld()->GetTimeSeconds() + IgnoreDurationOnStuck;
		StandDown();
		GEngine->AddOnScreenDebugMessage(20, 1.f, FColor::Purple, TEXT("Threat unreachable - doing something else"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(20, 0.2f, FColor::Orange,
		FString::Printf(TEXT("Engaging zombie - Dist: %.0f LOS: %d"), Dist, bLineOfSight ? 1 : 0));
}

void UThreatHandlerxrxs::HandleUnarmed(const FVector& DirToThreat, const FVector& Away)
{
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	AAIController* AI = GetAI();
	if (!Pawn)
		return;

	// Only sprint when a zombie is actually breathing down our neck
	const ABaseZombie* Nearest = GetNearestZombie(EngageExitRadius);
	const float Dist = Nearest ? FVector::Dist(Pawn->GetActorLocation(), Nearest->GetActorLocation()) : TNumericLimits<float>::Max();
	SetRunning(Dist < RunWhenWithin);

	// First choice: grab a dropped gun, as long as it isn't sitting toward the zombie
	if (UItemHandlerxrxs* Items = GetOwner()->FindComponentByClass<UItemHandlerxrxs>())
	{
		if (ABaseItem* Gun = Items->GetNearestWeapon(ArmUpSearchRadius))
		{
			const FVector ToGun = (Gun->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
			if (FVector::DotProduct(ToGun, DirToThreat) < 0.5f)
			{
				if (Items->TryCollect(Gun))
					return; // armed now, next tick we fight

				if (AI)
					MoveToActorTracked(Gun, 50.f);
				FaceInstant(ToGun);

				GEngine->AddOnScreenDebugMessage(20, 0.2f, FColor::Yellow, TEXT("Unarmed: running for a gun"));
				return;
			}
		}
	}

	// Otherwise run to the nearest house (guns spawn inside) unless it's toward the zombie
	if (UHouseTrackerxrxs* Houses = GetOwner()->FindComponentByClass<UHouseTrackerxrxs>())
	{
		if (AActor* House = Houses->GetNearestKnownHouse())
		{
			const FVector ToHouse = (House->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
			if (AI && FVector::DotProduct(ToHouse, DirToThreat) < 0.3f)
			{
				MoveToActorTracked(House, 200.f);
				FaceInstant(ToHouse);

				GEngine->AddOnScreenDebugMessage(20, 0.2f, FColor::Yellow, TEXT("Unarmed: fleeing to a house for a gun"));
				return;
			}
		}
	}

	// Last resort: just put distance between us and every zombie
	MoveAwayTo(Away);
	FaceInstant(Away);
	GEngine->AddOnScreenDebugMessage(20, 0.2f, FColor::Red, TEXT("Unarmed: fleeing"));
}

void UThreatHandlerxrxs::SetRunning(bool bRun)
{
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	if (!Pawn)
		return;

	if (bRun && !Pawn->IsRunning())
		Pawn->StartRunning();
	else if (!bRun && Pawn->IsRunning())
		Pawn->StopRunning();
}

void UThreatHandlerxrxs::StandDown()
{
	if (ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner()))
		Pawn->StopRunning();

	if (AAIController* AI = GetAI())
		AI->StopMovement();

	FireTimer = 0.f;
	MoveGoal = nullptr;
	MoveRefreshTimer = 0.f;
	StuckTime = 0.f;
	LastThreatDist = 1.0e9f;
}
