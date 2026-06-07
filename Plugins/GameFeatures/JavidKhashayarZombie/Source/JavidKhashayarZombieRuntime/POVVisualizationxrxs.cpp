// Fill out your copyright notice in the Description page of Project Settings.

#include "POVVisualizationxrxs.h"
#include "GameFramework/Pawn.h"

UPOVVisualizationxrxs::UPOVVisualizationxrxs()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.016f;
}

void UPOVVisualizationxrxs::BeginPlay()
{
	Super::BeginPlay();
}

void UPOVVisualizationxrxs::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDebugDraw)
	{
		DrawDebugCone();
	}
}

void UPOVVisualizationxrxs::DrawPOVCone()
{
	DrawDebugCone();
}

void UPOVVisualizationxrxs::DrawDebugCone()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
		return;

	FVector ConeOrigin = PawnOwner->GetActorLocation();
	FVector ConeDirection = GetPawnForwardVector();

	float HalfFOV = FOVAngle * 0.5f;
	// Use consistent SightRange (should match AI Perception sight radius)
	float CurrentSightRange = 5000.0f; 
	float RadiusAtDistance = CurrentSightRange * FMath::Tan(FMath::DegreesToRadians(HalfFOV));

	// Draw cone tip to base line
	FVector ConeBase = ConeOrigin + ConeDirection * CurrentSightRange;
	DrawDebugLine(GetWorld(), ConeOrigin, ConeBase, ConeColor, false, -1.f, 0, 1.f);

	// Draw cone edges (forming a cone shape)
	int32 NumSides = 16;
	float AngleDelta = 360.0f / NumSides;

	for (int32 i = 0; i < NumSides; ++i)
	{
		float Angle1 = FMath::DegreesToRadians(i * AngleDelta);
		float Angle2 = FMath::DegreesToRadians((i + 1) * AngleDelta);

		// Create cone edge points
		// Use a temporary rotation to find orthogonal vectors if ActorRight/Up are not available or consistent
		FVector Right, Up;
		ConeDirection.FindBestAxisVectors(Right, Up);

		FVector Point1 = ConeBase + (Right * FMath::Cos(Angle1) + Up * FMath::Sin(Angle1)) * RadiusAtDistance;
		FVector Point2 = ConeBase + (Right * FMath::Cos(Angle2) + Up * FMath::Sin(Angle2)) * RadiusAtDistance;

		// Draw cone outline edges
		DrawDebugLine(GetWorld(), ConeOrigin, Point1, ConeColor, false, -1.f, 0, 1.f);
		DrawDebugLine(GetWorld(), Point1, Point2, ConeColor, false, -1.f, 0, 0.5f);
	}

	// Draw forward direction line (more prominent)
	DrawDebugLine(GetWorld(), ConeOrigin, ConeBase, FColor::Cyan, false, -1.f, 0, 2.f);
}

FVector UPOVVisualizationxrxs::GetPawnForwardVector() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		return PawnOwner->GetActorForwardVector();
	}
	return FVector::ForwardVector;
}
