// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivorComponentInjectorxrxs.h"
#include "StudentPerceptorxrxs.h"
#include "HouseTrackerxrxs.h"
#include "SpiralMovementxrxs.h"
#include "POVVisualizationxrxs.h"
#include "ItemHandlerxrxs.h"
#include "ThreatHandlerxrxs.h"
#include "PurgeEscaperxrxs.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Pawn.h"

USurvivorComponentInjectorxrxs::USurvivorComponentInjectorxrxs()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USurvivorComponentInjectorxrxs::BeginPlay()
{
	Super::BeginPlay();
	InjectComponents();
}

void USurvivorComponentInjectorxrxs::InjectComponents()
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
	if (!Pawn->FindComponentByClass<UStudentPerceptorxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UHouseTrackerxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<USpiralMovementxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UPOVVisualizationxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UItemHandlerxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UThreatHandlerxrxs>()) bAllComponentsExist = false;
	if (!Pawn->FindComponentByClass<UPurgeEscaperxrxs>()) bAllComponentsExist = false;

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
	SetupItemHandler();
	SetupThreatHandler();
	SetupPurgeEscaper();

	bComponentsInjected = true;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		TEXT("All components injected successfully!"));
}

bool USurvivorComponentInjectorxrxs::AreComponentsInjected() const
{
	return bComponentsInjected;
}

void USurvivorComponentInjectorxrxs::SetupAIPerceptionComponent()
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

void USurvivorComponentInjectorxrxs::SetupStudentPerceptor()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UStudentPerceptorxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("StudentPerceptor already exists"));
		return;
	}

	StudentPerceptor = NewObject<UStudentPerceptorxrxs>(Pawn);
	StudentPerceptor->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("StudentPerceptor injected"));
}

void USurvivorComponentInjectorxrxs::SetupHouseTracker()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UHouseTrackerxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("HouseTracker already exists"));
		return;
	}

	HouseTracker = NewObject<UHouseTrackerxrxs>(Pawn);
	HouseTracker->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("HouseTracker injected"));
}

void USurvivorComponentInjectorxrxs::SetupSpiralMovement()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<USpiralMovementxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("SpiralMovement already exists"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("Creating SpiralMovement. Pawn: %s"), *Pawn->GetName()));

	SpiralMovement = NewObject<USpiralMovementxrxs>(Pawn);
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

void USurvivorComponentInjectorxrxs::SetupPOVVisualization()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UPOVVisualizationxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("POVVisualization already exists"));
		return;
	}

	POVVisualization = NewObject<UPOVVisualizationxrxs>(Pawn);
	POVVisualization->RegisterComponent();
	POVVisualization->bDebugDraw = true;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("POVVisualization injected"));
}

void USurvivorComponentInjectorxrxs::SetupItemHandler()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UItemHandlerxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("ItemHandler already exists"));
		return;
	}

	ItemHandler = NewObject<UItemHandlerxrxs>(Pawn);
	ItemHandler->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("ItemHandler injected"));
}

void USurvivorComponentInjectorxrxs::SetupThreatHandler()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UThreatHandlerxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("ThreatHandler already exists"));
		return;
	}

	ThreatHandler = NewObject<UThreatHandlerxrxs>(Pawn);
	ThreatHandler->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("ThreatHandler injected"));
}

void USurvivorComponentInjectorxrxs::SetupPurgeEscaper()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	if (Pawn->FindComponentByClass<UPurgeEscaperxrxs>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("PurgeEscaper already exists"));
		return;
	}

	PurgeEscaper = NewObject<UPurgeEscaperxrxs>(Pawn);
	PurgeEscaper->RegisterComponent();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		TEXT("PurgeEscaper injected"));
}
