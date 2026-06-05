// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "SpiralMovement.generated.h"

class UHouseTracker;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Spiraling,
	MovingToHouse,
	ExploringHouse
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API USpiralMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	USpiralMovement();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeSpiralMovement();
	FVector CalculateSpiralPosition(float Angle, float Radius);
	void UpdateMovement(float DeltaTime);
	void MoveToHouse(AActor* House);
	void ExploreHouse(float DeltaTime);
	void UpdateFacingDirection(const FVector& Direction);
	void MoveToPoint(const FVector& TargetPoint, float DeltaTime, float AcceptanceRadius = -1.0f);
	
	class APawn* GetPawnOwner() const { return PawnOwner; }

	// Where the spiral begins, near the center
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float SpiralStartRadius = 400.0f;

	// Distance between successive loops of the spiral
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float SpiralArmSpacing = 1200.0f;

	// Distance between the waypoints we step through along the spiral
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float SpiralPointSpacing = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float MovementSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float StoppingDistance = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Movement")
	float PointReachTimeout = 8.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Spiral Movement")
	EMovementState CurrentState = EMovementState::Spiraling;

private:
	FVector SpiralCenter;
	float CurrentAngle = 0.0f;
	float CurrentRadius;
	float CurrentPointReachTimer = 0.0f;
	float SpiralPointTimer = 0.0f;
	AActor* CurrentHouseTarget = nullptr;
	UHouseTracker* HouseTrackerComp = nullptr;
	class APawn* PawnOwner = nullptr;
	class AAIController* AIControllerOwner = nullptr;

	TArray<FVector> ExplorationPoints;
	int32 CurrentPointIndex = 0;
	FVector CurrentTargetPoint;

	bool bIsMovingTowardsSpiralPoint = false;
};
