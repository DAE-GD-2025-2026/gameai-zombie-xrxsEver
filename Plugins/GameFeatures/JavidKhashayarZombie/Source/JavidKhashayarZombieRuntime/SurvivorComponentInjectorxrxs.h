// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorComponentInjectorxrxs.generated.h"

class UStudentPerceptorxrxs;
class UHouseTrackerxrxs;
class USpiralMovementxrxs;
class UPOVVisualizationxrxs;
class UItemHandlerxrxs;
class UThreatHandlerxrxs;
class UPurgeEscaperxrxs;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API USurvivorComponentInjectorxrxs : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorComponentInjectorxrxs();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Survivor AI")
	void InjectComponents();

	UFUNCTION(BlueprintCallable, Category = "Survivor AI")
	bool AreComponentsInjected() const;

private:
	UStudentPerceptorxrxs* StudentPerceptor = nullptr;
	UHouseTrackerxrxs* HouseTracker = nullptr;
	USpiralMovementxrxs* SpiralMovement = nullptr;
	UPOVVisualizationxrxs* POVVisualization = nullptr;
	UItemHandlerxrxs* ItemHandler = nullptr;
	UThreatHandlerxrxs* ThreatHandler = nullptr;
	UPurgeEscaperxrxs* PurgeEscaper = nullptr;

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
