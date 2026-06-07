// Fill out your copyright notice in the Description page of Project Settings.

#include "PurgeEscaperxrxs.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "UObject/UnrealType.h"
#include "Survivor/SurvivorPawn.h"
#include "PurgeZones/PurgeZone.h"

UPurgeEscaperxrxs::UPurgeEscaperxrxs()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UPurgeEscaperxrxs::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Hysteresis: start escaping near the edge, keep going until well clear
	const float Edge = NearestEdgeDistance();
	if (!bEscaping && Edge <= EnterBuffer)
		bEscaping = true;
	else if (bEscaping && Edge > ExitBuffer)
		bEscaping = false;
}

AAIController* UPurgeEscaperxrxs::GetAI() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
}

float UPurgeEscaperxrxs::GetPurgeRadius(AActor* Zone) const
{
	if (!Zone)
		return 0.f;

	// Diameter is a protected UPROPERTY, so read it through reflection
	if (FFloatProperty* Prop = FindFProperty<FFloatProperty>(Zone->GetClass(), TEXT("Diameter")))
		return Prop->GetPropertyValue_InContainer(Zone) * 0.5f;

	FVector Origin, Extent;
	Zone->GetActorBounds(true, Origin, Extent);
	return FMath::Max(Extent.X, Extent.Y);
}

float UPurgeEscaperxrxs::NearestEdgeDistance() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return TNumericLimits<float>::Max();

	const FVector Me = Owner->GetActorLocation();
	float Nearest = TNumericLimits<float>::Max();
	for (TActorIterator<APurgeZone> It(World); It; ++It)
	{
		const float Edge = FVector::Dist2D(Me, It->GetActorLocation()) - GetPurgeRadius(*It);
		Nearest = FMath::Min(Nearest, Edge);
	}
	return Nearest;
}

bool UPurgeEscaperxrxs::IsInPurgeZone() const
{
	return bEscaping;
}

FVector UPurgeEscaperxrxs::OutwardDirection() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return FVector::ZeroVector;

	const FVector Me = Owner->GetActorLocation();
	FVector Sum = FVector::ZeroVector;
	for (TActorIterator<APurgeZone> It(World); It; ++It)
	{
		FVector Out = Me - It->GetActorLocation();
		Out.Z = 0.f;
		const float Dist = Out.Size();
		const float Radius = GetPurgeRadius(*It);
		if (Dist - Radius > ExitBuffer)
			continue; // far enough from this one to ignore

		const FVector OutDir = Dist > KINDA_SMALL_NUMBER ? Out / Dist : FVector::ForwardVector;
		Sum += OutDir * (Radius - Dist + ExitBuffer);
	}
	return Sum.GetSafeNormal();
}

bool UPurgeEscaperxrxs::PointIsSafe(const FVector& Point) const
{
	UWorld* World = GetWorld();
	if (!World)
		return false;

	for (TActorIterator<APurgeZone> It(World); It; ++It)
	{
		if (FVector::Dist2D(Point, It->GetActorLocation()) <= GetPurgeRadius(*It) + 150.f)
			return false;
	}
	return true;
}

FVector UPurgeEscaperxrxs::FindSafeEscapePoint() const
{
	const FVector Me = GetOwner()->GetActorLocation();
	FVector Base = OutwardDirection();
	if (Base.IsNearlyZero())
		Base = GetOwner()->GetActorForwardVector();

	// Travel far enough to clear the biggest zone we're near
	UWorld* World = GetWorld();
	float MaxRadius = 0.f;
	for (TActorIterator<APurgeZone> It(World); It; ++It)
		MaxRadius = FMath::Max(MaxRadius, GetPurgeRadius(*It));
	const float Travel = MaxRadius + ExitBuffer;

	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	// Sweep outwards-ish directions and pick the first that lands on safe navmesh
	const float Angles[] = { 0.f, 30.f, -30.f, 60.f, -60.f, 90.f, -90.f, 120.f, -120.f, 180.f };
	for (float Angle : Angles)
	{
		const FVector Dir = Base.RotateAngleAxis(Angle, FVector::UpVector);
		for (float Mult : { 1.0f, 1.6f })
		{
			FVector Target = Me + Dir * (Travel * Mult);
			if (Nav)
			{
				FNavLocation NavLoc;
				if (Nav->ProjectPointToNavigation(Target, NavLoc, FVector(400.f, 400.f, 500.f)))
					Target = NavLoc.Location;
				else
					continue;
			}
			if (PointIsSafe(Target))
				return Target;
		}
	}

	return Me + Base * Travel;
}

void UPurgeEscaperxrxs::EscapePurge(float DeltaTime)
{
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	AAIController* AI = GetAI();
	UWorld* World = GetWorld();
	if (!Pawn || !AI || !World)
		return;

	const FVector Me = Pawn->GetActorLocation();
	const FVector Target = FindSafeEscapePoint();

	Pawn->StartRunning();

	const FVector Dir = (Target - Me).GetSafeNormal();
	if (!Dir.IsNearlyZero())
	{
		FRotator Rot = Dir.Rotation();
		Rot.Pitch = 0.f;
		Rot.Roll = 0.f;
		Pawn->SetActorRotation(Rot);
	}

	// Don't restart the path every frame (that stalls the pawn)
	if (FVector::DistSquared(Target, LastMoveTarget) > 50.f * 50.f ||
		AI->GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		AI->MoveToLocation(Target, 50.f);
		LastMoveTarget = Target;
	}

	DrawDebugLine(World, Me, Target, FColor::Purple, false, 0.1f, 0, 3.f);
	GEngine->AddOnScreenDebugMessage(21, 0.2f, FColor::Purple, TEXT("Escaping purge zone!"));
}

void UPurgeEscaperxrxs::StopEscaping()
{
	if (ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner()))
		Pawn->StopRunning();

	if (AAIController* AI = GetAI())
		AI->StopMovement();

	LastMoveTarget = FVector(FLT_MAX);
}
