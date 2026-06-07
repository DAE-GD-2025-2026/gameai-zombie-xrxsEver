// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemHandler.h"
#include "EngineUtils.h"
#include "Items/BaseItem.h"
#include "Common/InventoryComponent.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Survivor/SurvivorPawn.h"

UItemHandler::UItemHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f;
}

void UItemHandler::BeginPlay()
{
	Super::BeginPlay();
}

void UItemHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Keep the inventory tidy and use medkit/food whenever we drop low
	ManageTimer += DeltaTime;
	if (ManageTimer >= 1.0f)
	{
		ManageTimer = 0.0f;
		ManageInventory();
	}
}

UInventoryComponent* UItemHandler::GetInventory() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UInventoryComponent>() : nullptr;
}

ASurvivorPawn* UItemHandler::GetSurvivor() const
{
	return Cast<ASurvivorPawn>(GetOwner());
}

bool UItemHandler::IsConsumable(EItemType Type) const
{
	return Type == EItemType::Food || Type == EItemType::Medkit;
}

bool UItemHandler::IsWeapon(EItemType Type) const
{
	return Type == EItemType::Pistol || Type == EItemType::Shotgun;
}

bool UItemHandler::IsTrash(ABaseItem* Item) const
{
	return !Item || Item->GetItemType() == EItemType::Garbage || Item->GetValue() <= 0;
}

ABaseItem* UItemHandler::SlotItem(int32 SlotIdx) const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv || SlotIdx < 0)
		return nullptr;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	return Items.IsValidIndex(SlotIdx) ? Items[SlotIdx] : nullptr;
}

int32 UItemHandler::CountOfType(EItemType Type) const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return 0;

	int32 Count = 0;
	for (ABaseItem* Item : Inv->GetInventory())
	{
		if (Item && Item->GetItemType() == Type)
			++Count;
	}
	return Count;
}

bool UItemHandler::HasType(EItemType Type) const
{
	return CountOfType(Type) > 0;
}

int32 UItemHandler::LowestValueOfType(EItemType Type) const
{
	const int32 Slot = FindLowestValueSlotOfType(Type);
	return Slot >= 0 ? SlotItem(Slot)->GetValue() : -1;
}

int32 UItemHandler::DesiredCount(EItemType Type) const
{
	// Keep a spare food bag - stamina drains constantly so we want more of it
	if (Type == EItemType::Food)
		return 2;
	if (Type == EItemType::Medkit || Type == EItemType::Pistol || Type == EItemType::Shotgun)
		return 1;
	return 0;
}

int32 UItemHandler::FindLowestValueSlotOfType(EItemType Type) const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return -1;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	int32 LowSlot = -1;
	int32 LowValue = TNumericLimits<int32>::Max();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] && Items[i]->GetItemType() == Type && Items[i]->GetValue() < LowValue)
		{
			LowValue = Items[i]->GetValue();
			LowSlot = i;
		}
	}
	return LowSlot;
}

int32 UItemHandler::FindFreeSlot() const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return -1;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] == nullptr)
			return i;
	}
	return -1;
}

int32 UItemHandler::FindGarbageOrSpentSlot() const
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv)
		return -1;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] && IsTrash(Items[i]))
			return i;
	}
	return -1;
}

bool UItemHandler::CanPlace(ABaseItem* Item) const
{
	if (!Item)
		return false;

	if (FindFreeSlot() >= 0)
		return true;

	const EItemType Type = Item->GetItemType();
	if ((IsWeapon(Type) || IsConsumable(Type)) && HasType(Type))
	{
		const int32 Low = FindLowestValueSlotOfType(Type);
		if (Low >= 0 && Item->GetValue() > SlotItem(Low)->GetValue())
			return true;
	}

	return FindGarbageOrSpentSlot() >= 0;
}

bool UItemHandler::IsDesirable(ABaseItem* Item) const
{
	if (!IsValid(Item) || Item->IsHidden())
		return false;

	const EItemType Type = Item->GetItemType();

	if (Type == EItemType::Garbage)
	{
		// Worth grabbing only to recycle the spawn pool, never at the cost of a real item
		return FindFreeSlot() >= 0 || FindGarbageOrSpentSlot() >= 0;
	}

	// Want it if we're still under our desired count of that type, or it beats our
	// weakest copy (more ammo / higher value, e.g. Food(5) over Food(2))...
	const bool bWantByType = (IsConsumable(Type) || IsWeapon(Type)) &&
		(CountOfType(Type) < DesiredCount(Type) || Item->GetValue() > LowestValueOfType(Type));

	// ...and only if we can actually find a slot for it (so the pickup never stalls)
	return bWantByType && CanPlace(Item);
}

ABaseItem* UItemHandler::GetBestItemToCollect() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return nullptr;

	const FVector MyLocation = Owner->GetActorLocation();
	const float SearchRadiusSq = SearchRadius * SearchRadius;

	ABaseItem* Best = nullptr;
	float BestDist = TNumericLimits<float>::Max();

	for (TActorIterator<ABaseItem> It(World); It; ++It)
	{
		ABaseItem* Item = *It;
		const float Dist = FVector::DistSquared(MyLocation, Item->GetActorLocation());
		if (Dist > SearchRadiusSq)
			continue;

		if (!IsDesirable(Item))
			continue;

		if (Dist < BestDist)
		{
			BestDist = Dist;
			Best = Item;
		}
	}

	return Best;
}

bool UItemHandler::HasItemToCollect() const
{
	return GetBestItemToCollect() != nullptr;
}

ABaseItem* UItemHandler::GetNearestWeapon(float Radius) const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
		return nullptr;

	const FVector MyLocation = Owner->GetActorLocation();
	const float RadiusSq = Radius * Radius;

	ABaseItem* Best = nullptr;
	float BestDist = TNumericLimits<float>::Max();

	for (TActorIterator<ABaseItem> It(World); It; ++It)
	{
		ABaseItem* Item = *It;
		if (!IsValid(Item) || Item->IsHidden() || Item->GetValue() <= 0)
			continue;

		if (!IsWeapon(Item->GetItemType()))
			continue;

		const float Dist = FVector::DistSquared(MyLocation, Item->GetActorLocation());
		if (Dist > RadiusSq)
			continue;

		if (Dist < BestDist)
		{
			BestDist = Dist;
			Best = Item;
		}
	}

	return Best;
}

bool UItemHandler::UseIfBeneficial(int32 SlotIdx)
{
	ABaseItem* Item = SlotItem(SlotIdx);
	ASurvivorPawn* Survivor = GetSurvivor();
	UInventoryComponent* Inv = GetInventory();
	if (!Item || !Survivor || !Inv || Item->GetValue() <= 0)
		return false;

	const EItemType Type = Item->GetItemType();

	if (Type == EItemType::Medkit)
	{
		if (UHealthComponent* Health = Survivor->FindComponentByClass<UHealthComponent>())
		{
			if (Health->GetHealth() < Health->GetMaxHealth())
			{
				Inv->UseItem(SlotIdx);
				return true;
			}
		}
	}
	else if (Type == EItemType::Food)
	{
		if (UStaminaComponent* Stamina = Survivor->FindComponentByClass<UStaminaComponent>())
		{
			if (Stamina->GetCurrentStamina() < Stamina->GetMaxStamina())
			{
				Inv->UseItem(SlotIdx);
				return true;
			}
		}
	}

	return false;
}

int32 UItemHandler::MakeRoomFor(ABaseItem* Item)
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv || !Item)
		return -1;

	int32 Slot = FindFreeSlot();
	if (Slot >= 0)
		return Slot;

	const EItemType Type = Item->GetItemType();

	// Already carry this type: replace our weaker copy (less ammo / lower value)
	if ((IsWeapon(Type) || IsConsumable(Type)) && HasType(Type))
	{
		const int32 Low = FindLowestValueSlotOfType(Type);
		if (Low >= 0 && Item->GetValue() > SlotItem(Low)->GetValue())
		{
			UseIfBeneficial(Low);
			Inv->RemoveItem(Low);
			return Low;
		}
	}

	// Otherwise dump trash to make space
	const int32 TrashSlot = FindGarbageOrSpentSlot();
	if (TrashSlot >= 0)
	{
		Inv->RemoveItem(TrashSlot);
		return TrashSlot;
	}

	return -1;
}

bool UItemHandler::TryCollect(ABaseItem* Item)
{
	UInventoryComponent* Inv = GetInventory();
	if (!Inv || !IsValid(Item) || Item->IsHidden())
		return false;

	// 2D distance: the item sits on the floor while our pivot is ~88uu up, so a
	// straight 3D check never gets under the pickup range even standing on it
	const float Dist = FVector::Dist2D(GetOwner()->GetActorLocation(), Item->GetActorLocation());
	if (Dist > Inv->GetPickupRange())
		return false;

	const int32 Slot = MakeRoomFor(Item);
	if (Slot < 0)
		return false;

	const bool bGrabbed = Inv->GrabItem(Slot, Item);
	if (bGrabbed)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
			FString::Printf(TEXT("Picked up %s into slot %d"), *Item->GetName(), Slot));
		ManageInventory();
	}
	return bGrabbed;
}

void UItemHandler::ManageInventory()
{
	UInventoryComponent* Inv = GetInventory();
	ASurvivorPawn* Survivor = GetSurvivor();
	if (!Inv || !Survivor)
		return;

	// Emergency: patch up / refuel when we drop low
	if (UHealthComponent* Health = Survivor->FindComponentByClass<UHealthComponent>())
	{
		if (Health->GetHealth() <= LowHealthThreshold)
		{
			const int32 Slot = FindLowestValueSlotOfType(EItemType::Medkit);
			if (Slot >= 0)
				Inv->UseItem(Slot);
		}
	}
	if (UStaminaComponent* Stamina = Survivor->FindComponentByClass<UStaminaComponent>())
	{
		if (Stamina->GetCurrentStamina() <= LowStaminaThreshold)
		{
			const int32 Slot = FindLowestValueSlotOfType(EItemType::Food);
			if (Slot >= 0)
				Inv->UseItem(Slot);
		}
	}

	// Throw out garbage and anything spent so the slot frees up (and the pool recycles)
	for (int32 i = 0; i < Inv->GetInventoryCapacity(); ++i)
	{
		if (ABaseItem* Item = SlotItem(i); Item && IsTrash(Item))
		{
			Inv->RemoveItem(i);
		}
	}

	// Trim down to our desired count per type (1 gun/medkit, 2 food), keeping the best
	const EItemType Types[] = { EItemType::Pistol, EItemType::Shotgun, EItemType::Medkit, EItemType::Food };
	for (EItemType Type : Types)
	{
		while (CountOfType(Type) > DesiredCount(Type))
		{
			const int32 Low = FindLowestValueSlotOfType(Type);
			if (Low < 0)
				break;

			UseIfBeneficial(Low);
			Inv->RemoveItem(Low);
		}
	}
}
