// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorComponentInjector.generated.h"

class UStudentPerceptor;
class UHouseTracker;
class USpiralMovement;
class UPOVVisualization;
class UItemHandler;
class UThreatHandler;
class UPurgeEscaper;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API USurvivorComponentInjector : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorComponentInjector();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Survivor AI")
	void InjectComponents();

	UFUNCTION(BlueprintCallable, Category = "Survivor AI")
	bool AreComponentsInjected() const;

private:
	UStudentPerceptor* StudentPerceptor = nullptr;
	UHouseTracker* HouseTracker = nullptr;
	USpiralMovement* SpiralMovement = nullptr;
	UPOVVisualization* POVVisualization = nullptr;
	UItemHandler* ItemHandler = nullptr;
	UThreatHandler* ThreatHandler = nullptr;
	UPurgeEscaper* PurgeEscaper = nullptr;

	void SetupStudentPerceptor();
	void SetupHouseTracker();
	void SetupSpiralMovement();
	void SetupPOVVisualization();
	void SetupAIPerceptionComponent();
	void SetupItemHandler();
	void SetupThreatHandler();
	void SetupPurgeEscaper();

	bool bComponentsInjected = false;
};
