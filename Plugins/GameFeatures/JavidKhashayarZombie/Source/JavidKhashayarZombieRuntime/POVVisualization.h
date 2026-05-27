// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POVVisualization.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UPOVVisualization : public UActorComponent
{
	GENERATED_BODY()

public:
	UPOVVisualization();

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
