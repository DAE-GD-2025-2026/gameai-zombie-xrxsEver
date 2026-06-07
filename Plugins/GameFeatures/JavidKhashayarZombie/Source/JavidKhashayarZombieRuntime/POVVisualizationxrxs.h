// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POVVisualizationxrxs.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UPOVVisualizationxrxs : public UActorComponent
{
	GENERATED_BODY()

public:
	UPOVVisualizationxrxs();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DrawPOVCone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POV Visualization")
	float SightRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POV Visualization")
	float FOVAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POV Visualization")
	FColor ConeColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POV Visualization")
	bool bDebugDraw = true;

private:
	void DrawDebugCone();
	FVector GetPawnForwardVector() const;
};
