// Fill out your copyright notice in the description page of Project Settings.

#include "SpiralMovement.h"
#include "HouseTracker.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DrawDebugHelpers.h"

USpiralMovement::USpiralMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.016f;
	StoppingDistance = 150.0f;
	PointReachTimeout = 8.0f;
}

void USpiralMovement::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
		FString::Printf(TEXT("SpiralMovement BeginPlay called. Owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("NULL")));

	PawnOwner = Cast<APawn>(GetOwner());
	AIControllerOwner = PawnOwner ? Cast<AAIController>(PawnOwner->GetController()) : nullptr;
	HouseTrackerComp = GetOwner()->FindComponentByClass<UHouseTracker>();

	if (!PawnOwner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: PawnOwner is NULL after cast!"));
	}

	InitializeSpiralMovement();
}

void USpiralMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner)
	{
		PawnOwner = Cast<APawn>(GetOwner());
		if (PawnOwner)
		{
			AIControllerOwner = Cast<AAIController>(PawnOwner->GetController());
		}
		
		if (!PawnOwner)
		{
			GEngine->AddOnScreenDebugMessage(10, 0.05f, FColor::Red, TEXT("SpiralMovement: No pawn owner!"));
			return;
		}
	}

	if (!AIControllerOwner && PawnOwner)
	{
		AIControllerOwner = Cast<AAIController>(PawnOwner->GetController());
	}

	if (!HouseTrackerComp)
	{
		HouseTrackerComp = GetOwner() ? GetOwner()->FindComponentByClass<UHouseTracker>() : nullptr;
	}

	UpdateMovement(DeltaTime);
}

void USpiralMovement::InitializeSpiralMovement()
{
	if (PawnOwner)
	{
		SpiralCenter = PawnOwner->GetActorLocation();
		CurrentRadius = SpiralRadius;
		CurrentAngle = 0.0f;
	}
}

FVector USpiralMovement::CalculateSpiralPosition(float Angle, float Radius)
{
	float X = SpiralCenter.X + Radius * FMath::Cos(FMath::DegreesToRadians(Angle));
	float Y = SpiralCenter.Y + Radius * FMath::Sin(FMath::DegreesToRadians(Angle));
	return FVector(X, Y, SpiralCenter.Z);
}

void USpiralMovement::UpdateMovement(float DeltaTime)
{
	if (!PawnOwner)
	{
		return;
	}

	UPawnMovementComponent* MovementComp = PawnOwner->FindComponentByClass<UPawnMovementComponent>();
	if (!MovementComp)
	{
		GEngine->AddOnScreenDebugMessage(11, 0.05f, FColor::Red, TEXT("No Movement component!"));
		return;
	}

	if (CurrentState == EMovementState::Spiraling)
	{
		// Internal transition check removed as BT now drives this
		// We still keep the spiraling logic
		CurrentAngle += SpiralAngularVelocity * DeltaTime;
		CurrentRadius += (SpiralRadiusIncrement / 360.0f) * SpiralAngularVelocity * DeltaTime;

		FVector TargetPos = CalculateSpiralPosition(CurrentAngle, CurrentRadius);
		MoveToPoint(TargetPos, DeltaTime);

		GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Blue,
			FString::Printf(TEXT("Spiraling - Angle: %.1f, Radius: %.0f"), CurrentAngle, CurrentRadius));
	}
	else if (CurrentState == EMovementState::MovingToHouse)
	{
		if (!CurrentHouseTarget)
		{
			CurrentState = EMovementState::Spiraling;
			return;
		}

		FVector HouseCenter = CurrentHouseTarget->GetActorLocation();
		float DistanceToHouse = FVector::Dist(PawnOwner->GetActorLocation(), HouseCenter);

		MoveToPoint(HouseCenter, DeltaTime);
		CurrentPointReachTimer += DeltaTime;

		GEngine->AddOnScreenDebugMessage(2, 0.05f, FColor::Red,
			FString::Printf(TEXT("Moving to House - Distance: %.0f, Timer: %.1f"), DistanceToHouse, CurrentPointReachTimer));

		if (DistanceToHouse <= (StoppingDistance + 50.f) || CurrentPointReachTimer >= PointReachTimeout)
		{
			if (HouseTrackerComp)
			{
				ExplorationPoints = HouseTrackerComp->GenerateExplorationPoints(CurrentHouseTarget);
				CurrentPointIndex = 0;
				CurrentPointReachTimer = 0.0f;
				if (ExplorationPoints.Num() > 0)
				{
					CurrentTargetPoint = ExplorationPoints[0];
					CurrentState = EMovementState::ExploringHouse;
				}
				else
				{
					CurrentState = EMovementState::Spiraling;
				}
			}
			else
			{
				CurrentState = EMovementState::Spiraling;
			}
		}
	}
	else if (CurrentState == EMovementState::ExploringHouse)
	{
		ExploreHouse(DeltaTime);
	}
}

void USpiralMovement::MoveToHouse(AActor* House)
{
	CurrentHouseTarget = House;
	CurrentState = EMovementState::MovingToHouse;
	CurrentPointReachTimer = 0.0f;
}

void USpiralMovement::ExploreHouse(float DeltaTime)
{
	if (!PawnOwner || !CurrentHouseTarget || ExplorationPoints.Num() == 0)
	{
		CurrentState = EMovementState::Spiraling;
		return;
	}

	float DistanceToPoint = FVector::Dist(PawnOwner->GetActorLocation(), CurrentTargetPoint);
	
	GEngine->AddOnScreenDebugMessage(3, 0.05f, FColor::Yellow,
		FString::Printf(TEXT("Exploring House Point %d/%d - Distance: %.0f, Timer: %.1f"), 
		CurrentPointIndex + 1, ExplorationPoints.Num(), DistanceToPoint, CurrentPointReachTimer));

	// Use a smaller acceptance radius for exploration to ensure we "pass through" the points
	MoveToPoint(CurrentTargetPoint, DeltaTime, 50.0f);
	CurrentPointReachTimer += DeltaTime;

	if (DistanceToPoint <= 100.0f || CurrentPointReachTimer >= PointReachTimeout)
	{
		CurrentPointReachTimer = 0.0f;
		CurrentPointIndex++;

		if (CurrentPointIndex < ExplorationPoints.Num())
		{
			CurrentTargetPoint = ExplorationPoints[CurrentPointIndex];
		}
		else
		{
			if (HouseTrackerComp)
			{
				HouseTrackerComp->MarkHouseVisited(CurrentHouseTarget);
			}
			CurrentHouseTarget = nullptr;
			CurrentState = EMovementState::Spiraling;
		}
	}
}

void USpiralMovement::MoveToPoint(const FVector& TargetPoint, float DeltaTime, float AcceptanceRadius)
{
	if (!PawnOwner) return;

	float ActualAcceptanceRadius = (AcceptanceRadius < 0.0f) ? StoppingDistance : AcceptanceRadius;

	if (AIControllerOwner)
	{
		// Use MoveToLocation which handles pathfinding via NavMesh
		AIControllerOwner->MoveToLocation(TargetPoint, ActualAcceptanceRadius);
		
		// Debug draw the current path/target
		DrawDebugLine(GetWorld(), PawnOwner->GetActorLocation(), TargetPoint, FColor::Yellow, false, 0.05f, 0, 2.f);
	}
	else
	{
		// Fallback to simple direction if no AI controller
		FVector Direction = (TargetPoint - PawnOwner->GetActorLocation()).GetSafeNormal();
		PawnOwner->AddMovementInput(Direction, 1.0f);
	}
	
	// Ensure we are facing the direction of movement/velocity
	FVector CurrentVelocity = PawnOwner->GetVelocity();
	if (!CurrentVelocity.IsNearlyZero())
	{
		UpdateFacingDirection(CurrentVelocity.GetSafeNormal());
	}
	else
	{
		FVector Direction = (TargetPoint - PawnOwner->GetActorLocation()).GetSafeNormal();
		UpdateFacingDirection(Direction);
	}
}

void USpiralMovement::UpdateFacingDirection(const FVector& Direction)
{
	if (!PawnOwner || Direction.IsNearlyZero())
	{
		return;
	}

	FRotator CurrentRotation = PawnOwner->GetActorRotation();
	FRotator TargetRotation = Direction.Rotation();
	
	// Only update Yaw for a survivor character usually
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
	PawnOwner->SetActorRotation(NewRotation);
}
