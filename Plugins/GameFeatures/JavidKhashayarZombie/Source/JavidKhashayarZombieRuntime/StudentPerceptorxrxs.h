// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptorxrxs.generated.h"

class UHouseTrackerxrxs;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UStudentPerceptorxrxs : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptorxrxs();

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Detection")
	FString HouseActorTag = TEXT("House");

private:
	UHouseTrackerxrxs* HouseTrackerComp = nullptr;

	bool IsHouse(AActor* Actor) const;
};
