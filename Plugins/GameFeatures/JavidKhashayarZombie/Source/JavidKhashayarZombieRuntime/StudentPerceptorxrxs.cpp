// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptorxrxs.h"
#include "HouseTrackerxrxs.h"

UStudentPerceptorxrxs::UStudentPerceptorxrxs()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptorxrxs::BeginPlay()
{
	Super::BeginPlay();

	HouseTrackerComp = GetOwner()->FindComponentByClass<UHouseTrackerxrxs>();

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorxrxs::OnPerceptionUpdated);
	}
}

void UStudentPerceptorxrxs::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
		return;

	// Ensure we have the house tracker
	if (!HouseTrackerComp)
	{
		HouseTrackerComp = GetOwner() ? GetOwner()->FindComponentByClass<UHouseTrackerxrxs>() : nullptr;
	}

	// Debug log for every perceived actor to see what we are sensing
	if (Stimulus.WasSuccessfullySensed())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White,
			FString::Printf(TEXT("Sensed: %s (Class: %s)"), *Actor->GetName(), *Actor->GetClass()->GetName()));
	}

	if (Stimulus.WasSuccessfullySensed() && IsHouse(Actor))
	{
		if (HouseTrackerComp)
		{
			HouseTrackerComp->AddHouseToVisit(Actor);
		}

		FVector HouseCenter = Actor->GetActorLocation();
		DrawDebugSphere(GetWorld(), HouseCenter, 100.f, 12, FColor::Yellow, false, 2.f);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("DETECTED HOUSE: %s at (%.0f, %.0f, %.0f)"), *Actor->GetName(), HouseCenter.X, HouseCenter.Y, HouseCenter.Z));
	}
}

bool UStudentPerceptorxrxs::IsHouse(AActor* Actor) const
{
	if (!Actor)
		return false;

	// 1. Check by tag
	if (Actor->ActorHasTag(*HouseActorTag))
	{
		return true;
	}

	// 2. Check by Class Name (Robust fallback without direct dependency)
	if (Actor->GetClass()->GetName().Contains(TEXT("House")))
	{
		return true;
	}

	// 3. Check by Actor Name (Last resort fallback)
	if (Actor->GetName().Contains(TEXT("House")))
	{
		return true;
	}

	return false;
}
