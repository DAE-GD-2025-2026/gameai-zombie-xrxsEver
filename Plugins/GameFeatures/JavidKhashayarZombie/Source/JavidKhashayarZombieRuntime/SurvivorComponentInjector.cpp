// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivorComponentInjector.h"
#include "StudentPerceptor.h"
#include "HouseTracker.h"
#include "SpiralMovement.h"
#include "POVVisualization.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Pawn.h"

USurvivorComponentInjector::USurvivorComponentInjector()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USurvivorComponentInjector::BeginPlay()
{
	Super::BeginPlay();
	InjectComponents();
}

void USurvivorComponentInjector::InjectComponents()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("InjectComponents() called!"));

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			TEXT("SurvivorComponentInjector: Owner is not a Pawn!"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
		FString::Printf(TEXT("Owner is Pawn: %s"), *Pawn->GetName()));

	// Check if components already exist to avoid duplicates
	bool bAllComponentsExist = true;
	if (!Pawn->FindComponentByClass<UStudentPerceptor>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UHouseTracker>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<USpiralMovement>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UPOVVisualization>()) bAllComponentsExist = false;

	if (bAllComponentsExist)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("Survivor AI components already fully injected"));
		
		// Ensure perception is configured even if components already exist
		SetupAIPerceptionComponent();
		
		bComponentsInjected = true;
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		TEXT("Injecting Survivor AI components..."));

	SetupHouseTracker();
	SetupSpiralMovement();
	SetupAIPerceptionComponent();
	SetupStudentPerceptor();
	SetupPOVVisualization();

	bComponentsInjected = true;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		TEXT("All components injected successfully!"));
}

bool USurvivorComponentInjector::AreComponentsInjected() const
{
	return bComponentsInjected;
}

void USurvivorComponentInjector::SetupAIPerceptionComponent()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UAIPerceptionComponent>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("AIPerceptionComponent already exists"));
	}
	else
	{
		UAIPerceptionComponent* PerceptionComp = NewObject<UAIPerceptionComponent>(Pawn);
		PerceptionComp->RegisterComponent();

		UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>();
		SightConfig->SightRadius = 5000.0f;
		SightConfig->LoseSightRadius = 5500.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		PerceptionComp->ConfigureSense(*SightConfig);

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
			TEXT("AIPerceptionComponent created"));
	}
}

void USurvivorComponentInjector::SetupStudentPerceptor()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UStudentPerceptor>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("StudentPerceptor already exists"));
		return;
	}

	StudentPerceptor = NewObject<UStudentPerceptor>(Pawn);
	StudentPerceptor->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("StudentPerceptor injected"));
}

void USurvivorComponentInjector::SetupHouseTracker()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UHouseTracker>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("HouseTracker already exists"));
		return;
	}

	HouseTracker = NewObject<UHouseTracker>(Pawn);
	HouseTracker->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("HouseTracker injected"));
}

void USurvivorComponentInjector::SetupSpiralMovement()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<USpiralMovement>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("SpiralMovement already exists"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("Creating SpiralMovement. Pawn: %s"), *Pawn->GetName()));

	SpiralMovement = NewObject<USpiralMovement>(Pawn);
	SpiralMovement->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("Registered SpiralMovement. Owner: %s, TickEnabled: %d"),
		SpiralMovement->GetOwner() ? *SpiralMovement->GetOwner()->GetName() : TEXT("NULL"),
		SpiralMovement->IsComponentTickEnabled()));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("PawnOwner set: %s"),
		SpiralMovement->GetPawnOwner() ? *SpiralMovement->GetPawnOwner()->GetName() : TEXT("NULL")));

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("SpiralMovement injected"));
}

void USurvivorComponentInjector::SetupPOVVisualization()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UPOVVisualization>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("POVVisualization already exists"));
		return;
	}

	POVVisualization = NewObject<UPOVVisualization>(Pawn);
	POVVisualization->RegisterComponent();
	POVVisualization->bDebugDraw = true;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("POVVisualization injected"));
}
