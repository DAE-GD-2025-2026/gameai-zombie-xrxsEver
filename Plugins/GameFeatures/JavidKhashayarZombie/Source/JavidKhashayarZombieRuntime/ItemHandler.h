// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemType.h"
#include "ItemHandler.generated.h"

class ABaseItem;
class UInventoryComponent;
class ASurvivorPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAVIDKHASHAYARZOMBIERUNTIME_API UItemHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemHandler();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ABaseItem* GetBestItemToCollect() const;
	bool HasItemToCollect() const;
	bool TryCollect(ABaseItem* Item);

	// Nearest pistol/shotgun lying on the ground (used by threat handling to go arm up)
	ABaseItem* GetNearestWeapon(float Radius) const;

	// Decide what to do with everything we are carrying: use, keep, or trash
	void ManageInventory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Handling")
	float SearchRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Handling")
	int32 LowHealthThreshold = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Handling")
	float LowStaminaThreshold = 3.0f;

private:
	bool IsConsumable(EItemType Type) const;
	bool IsWeapon(EItemType Type) const;
	bool IsTrash(ABaseItem* Item) const;
	bool IsDesirable(ABaseItem* Item) const;
	bool CanPlace(ABaseItem* Item) const;

	int32 CountOfType(EItemType Type) const;
	bool HasType(EItemType Type) const;
	int32 LowestValueOfType(EItemType Type) const;
	int32 DesiredCount(EItemType Type) const;
	int32 FindLowestValueSlotOfType(EItemType Type) const;
	int32 FindFreeSlot() const;
	int32 FindGarbageOrSpentSlot() const;
	ABaseItem* SlotItem(int32 SlotIdx) const;

	// Free up a slot for the incoming item, returns the slot index or -1 if not worth it
	int32 MakeRoomFor(ABaseItem* Item);
	// Use a held consumable only if it actually improves our condition; returns true if used
	bool UseIfBeneficial(int32 SlotIdx);

	UInventoryComponent* GetInventory() const;
	ASurvivorPawn* GetSurvivor() const;

	float ManageTimer = 0.0f;
};
