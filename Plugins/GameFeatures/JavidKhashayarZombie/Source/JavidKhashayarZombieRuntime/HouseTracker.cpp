// Fill out your copyright notice in the Description page of Project Settings.

#include "HouseTracker.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UHouseTracker::UHouseTracker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHouseTracker::BeginPlay()
{
	Super::BeginPlay();
}

void UHouseTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHouseTracker::AddHouseToVisit(AActor* House)
{
	if (House && !HousesToVisit.Contains(House) && !VisitedHouses.Contains(House))
	{
		HousesToVisit.Add(House);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			FString::Printf(TEXT("House added to visit list. Total: %d"), HousesToVisit.Num()));
	}
}

AActor* UHouseTracker::GetNextHouseTarget()
{
	if (HousesToVisit.Num() > 0)
	{
		return HousesToVisit[0];
	}
	return nullptr;
}

bool UHouseTracker::HasHousesToVisit() const
{
	return HousesToVisit.Num() > 0;
}

void UHouseTracker::MarkHouseVisited(AActor* House)
{
	if (House)
	{
		HousesToVisit.Remove(House);
		VisitedHouses.Add(House);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
			FString::Printf(TEXT("House visited! Remaining: %d"), HousesToVisit.Num()));

		if (HousesToVisit.Num() == 0 && VisitedHouses.Num() > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("All houses visited! Resetting list to visit again..."));
			//ResetVisitedHouses();
		}
	}
}

void UHouseTracker::ResetVisitedHouses()
{
	for (AActor* House : VisitedHouses)
	{
		if (House)
		{
			HousesToVisit.Add(House);
		}
	}
	VisitedHouses.Empty();
}

TArray<FVector> UHouseTracker::GenerateExplorationPoints(AActor* House)
{
	TArray<FVector> Points;
	if (!House) return Points;

	FVector Origin, BoxExtent;
	House->GetActorBounds(true, Origin, BoxExtent);
	
	FVector Center = House->GetActorLocation();
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	auto AddProjectedPoint = [&](FVector Point) {
		if (NavSys)
		{
			FNavLocation NavLoc;
			// Project point to navigation with some vertical tolerance
			if (NavSys->ProjectPointToNavigation(Point, NavLoc, FVector(100.f, 100.f, 500.f)))
			{
				Points.Add(NavLoc.Location);
				// Debug draw the target point
				DrawDebugSphere(GetWorld(), NavLoc.Location, 50.f, 8, FColor::Yellow, false, 5.f);
				return;
			}
		}
		// Fallback if NavSys fails or not available
		Points.Add(Point);
		DrawDebugSphere(GetWorld(), Point, 50.f, 8, FColor::Red, false, 5.f);
	};

	// Skip center point if it's likely to be blocked, circular points provide better coverage
	// AddProjectedPoint(Center);

	// Use a more balanced radius: 60% of extent, but clamped to avoid extreme sizes
	// This ensures points are neither too close to the center nor outside small houses
	float RadiusX = FMath::Clamp(BoxExtent.X * 0.6f, 200.f, 600.f);
	float RadiusY = FMath::Clamp(BoxExtent.Y * 0.6f, 200.f, 600.f);
	
	// Final safety check: ensure the radius isn't larger than 80% of the actual box extent
	// (Prevents points from being forced outside very small shacks by the 200.f clamp)
	RadiusX = FMath::Min(RadiusX, BoxExtent.X * 0.8f);
	RadiusY = FMath::Min(RadiusY, BoxExtent.Y * 0.8f);

	const int32 NumPoints = 8;
	for (int32 i = 0; i < NumPoints; ++i)
	{
		float Angle = (360.0f / NumPoints) * i;
		float Rad = FMath::DegreesToRadians(Angle);
		
		FVector Offset(FMath::Cos(Rad) * RadiusX, FMath::Sin(Rad) * RadiusY, 0);
		AddProjectedPoint(Center + Offset);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, 
		FString::Printf(TEXT("Generated %d exploration points for %s (Radius: %.0f, %.0f)"), 
		Points.Num(), *House->GetName(), RadiusX, RadiusY));

	return Points;
}
