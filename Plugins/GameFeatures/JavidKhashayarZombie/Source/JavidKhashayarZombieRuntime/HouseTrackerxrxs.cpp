// Fill out your copyright notice in the Description page of Project Settings.

#include "HouseTrackerxrxs.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

UHouseTrackerxrxs::UHouseTrackerxrxs()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f;
}

void UHouseTrackerxrxs::BeginPlay()
{
	Super::BeginPlay();
	DiscoverHouses();
}

void UHouseTrackerxrxs::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Villages spawn at game start, but ordering with injection isn't guaranteed,
	// so keep rescanning until we've found something and refresh occasionally after.
	RescanTimer += DeltaTime;
	if (RescanTimer >= RescanInterval && (KnownHouses.Num() == 0 || RescanTimer >= RescanInterval * 4.f))
	{
		RescanTimer = 0.0f;
		DiscoverHouses();
	}
}

void UHouseTrackerxrxs::DiscoverHouses()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	int32 Added = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (IsHouseActor(Actor) && !KnownHouses.Contains(Actor))
		{
			KnownHouses.Add(Actor);
			++Added;
		}
	}

	if (Added > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("Discovered %d new houses (total %d)"), Added, KnownHouses.Num()));
	}
}

bool UHouseTrackerxrxs::IsHouseActor(AActor* Actor) const
{
	if (!Actor)
		return false;

	if (Actor->ActorHasTag(*HouseActorTag))
		return true;
	if (Actor->GetClass()->GetName().Contains(TEXT("House")))
		return true;
	if (Actor->GetName().Contains(TEXT("House")))
		return true;

	return false;
}

void UHouseTrackerxrxs::AddHouseToVisit(AActor* House)
{
	if (House && IsHouseActor(House) && !KnownHouses.Contains(House))
	{
		KnownHouses.Add(House);
	}
}

bool UHouseTrackerxrxs::IsOnCooldown(AActor* House) const
{
	const int32* Visited = VisitedAtCount.Find(House);
	if (!Visited)
		return false;

	return (VisitCounter - *Visited) < RevisitCooldownVisits;
}

AActor* UHouseTrackerxrxs::GetNextHouseTarget()
{
	const AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	const FVector MyLocation = Owner->GetActorLocation();

	AActor* BestEligible = nullptr;
	float BestEligibleDist = TNumericLimits<float>::Max();

	// Fallback when everything is on cooldown: the house we visited longest ago
	AActor* Oldest = nullptr;
	int32 OldestVisit = TNumericLimits<int32>::Max();

	for (AActor* House : KnownHouses)
	{
		if (!House)
			continue;

		if (IsOnCooldown(House))
		{
			const int32 Visited = VisitedAtCount.FindChecked(House);
			if (Visited < OldestVisit)
			{
				OldestVisit = Visited;
				Oldest = House;
			}
			continue;
		}

		const float Dist = FVector::DistSquared(MyLocation, House->GetActorLocation());
		if (Dist < BestEligibleDist)
		{
			BestEligibleDist = Dist;
			BestEligible = House;
		}
	}

	return BestEligible ? BestEligible : Oldest;
}

bool UHouseTrackerxrxs::HasHousesToVisit() const
{
	return KnownHouses.Num() > 0;
}

AActor* UHouseTrackerxrxs::GetNearestKnownHouse() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	const FVector MyLocation = Owner->GetActorLocation();
	AActor* Nearest = nullptr;
	float NearestDist = TNumericLimits<float>::Max();

	for (AActor* House : KnownHouses)
	{
		if (!House)
			continue;

		const float Dist = FVector::DistSquared(MyLocation, House->GetActorLocation());
		if (Dist < NearestDist)
		{
			NearestDist = Dist;
			Nearest = House;
		}
	}

	return Nearest;
}

void UHouseTrackerxrxs::MarkHouseVisited(AActor* House)
{
	if (!House)
		return;

	++VisitCounter;
	VisitedAtCount.Add(House, VisitCounter);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
		FString::Printf(TEXT("House visited (#%d). Next revisit allowed in %d visits"), VisitCounter, RevisitCooldownVisits));
}

void UHouseTrackerxrxs::ResetVisitedHouses()
{
	VisitedAtCount.Empty();
	VisitCounter = 0;
}

TArray<FVector> UHouseTrackerxrxs::GenerateExplorationPoints(AActor* House)
{
	TArray<FVector> Points;
	if (!House)
		return Points;

	FVector Origin, Extent;
	House->GetActorBounds(true, Origin, Extent);

	// Stay off the walls by insetting the usable area
	const float MarginX = FMath::Min(Extent.X * 0.25f, 200.f);
	const float MarginY = FMath::Min(Extent.Y * 0.25f, 200.f);
	const float UsableX = FMath::Max(Extent.X - MarginX, 0.f);
	const float UsableY = FMath::Max(Extent.Y - MarginY, 0.f);

	// Coarse grid: a few points is enough since we grab items on the way (within item search range)
	const float Spacing = 600.f;
	const int32 CountX = FMath::Clamp(FMath::CeilToInt((UsableX * 2.f) / Spacing) + 1, 1, 3);
	const int32 CountY = FMath::Clamp(FMath::CeilToInt((UsableY * 2.f) / Spacing) + 1, 1, 3);

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	for (int32 Row = 0; Row < CountY; ++Row)
	{
		const float AlphaY = (CountY == 1) ? 0.5f : (float)Row / (CountY - 1);
		const float OffsetY = FMath::Lerp(-UsableY, UsableY, AlphaY);

		// Serpentine so the path sweeps instead of jumping across the house
		for (int32 Col = 0; Col < CountX; ++Col)
		{
			const int32 RealCol = (Row % 2 == 0) ? Col : (CountX - 1 - Col);
			const float AlphaX = (CountX == 1) ? 0.5f : (float)RealCol / (CountX - 1);
			const float OffsetX = FMath::Lerp(-UsableX, UsableX, AlphaX);

			const FVector Desired(Origin.X + OffsetX, Origin.Y + OffsetY, Origin.Z);

			if (NavSys)
			{
				FNavLocation NavLoc;
				// Tight tolerance so we don't snap onto navmesh outside the walls
				if (NavSys->ProjectPointToNavigation(Desired, NavLoc, FVector(80.f, 80.f, Extent.Z + 100.f)))
				{
					if (FVector::Dist2D(NavLoc.Location, Desired) <= 160.f)
					{
						Points.Add(NavLoc.Location);
						DrawDebugSphere(GetWorld(), NavLoc.Location, 40.f, 8, FColor::Yellow, false, 5.f);
					}
				}
			}
			else
			{
				Points.Add(Desired);
				DrawDebugSphere(GetWorld(), Desired, 40.f, 8, FColor::Red, false, 5.f);
			}
		}
	}

	// Always have at least the center so a house is never skipped
	if (Points.Num() == 0)
	{
		FVector Center = Origin;
		if (NavSys)
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(Origin, NavLoc, FVector(200.f, 200.f, Extent.Z + 100.f)))
				Center = NavLoc.Location;
		}
		Points.Add(Center);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("Generated %d exploration points for %s (grid %dx%d)"),
		Points.Num(), *House->GetName(), CountX, CountY));

	return Points;
}
