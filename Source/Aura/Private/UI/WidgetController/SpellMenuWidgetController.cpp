// Copyright Giant Rocket Games


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	OnPlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStateChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StateTag, int32 AbilityLevel){
		
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.StateTag = StateTag;

			bool bEnableSpendButton = false;
			bool bEnableEquipButton = false;
			ShouldEnableButtons(StateTag, CurrentSpellPoints, bEnableSpendButton, bEnableEquipButton);

			FString DescriptionString;
			FString NextLevelDescriptionString;

			GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, DescriptionString, NextLevelDescriptionString);

			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, DescriptionString, NextLevelDescriptionString);
		}
		
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StateTag = StateTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->SpellPointsChangedDelegate.AddLambda([this](int32 Points) {
		CurrentSpellPoints = Points;
		OnPlayerSpellPointsChanged.Broadcast(Points);

		bool bEnableSpendButton = false;
		bool bEnableEquipButton = false;
		ShouldEnableButtons(SelectedAbility.StateTag, CurrentSpellPoints, bEnableSpendButton, bEnableEquipButton);

		FString DescriptionString;
		FString NextLevelDescriptionString;

		GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, DescriptionString, NextLevelDescriptionString);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, DescriptionString, NextLevelDescriptionString);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityState;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bIsNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bTagValid || bIsNone || !bSpecValid)
	{
		AbilityState = GameplayTags.Abilities_State_Locked;
	}
	else
	{
		AbilityState = GetAuraASC()->GetStateFromSpec(*AbilitySpec);
	}

	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StateTag = AbilityState;

	bool bEnableSpendButton = false;
	bool bEnableEquipButton = false;
	ShouldEnableButtons(AbilityState, SpellPoints, bEnableSpendButton, bEnableEquipButton);

	FString DescriptionString;
	FString NextLevelDescriptionString;

	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, DescriptionString, NextLevelDescriptionString);

	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, DescriptionString, NextLevelDescriptionString);
}

void USpellMenuWidgetController::SpellGlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StateTag = FAuraGameplayTags::Get().Abilities_State_Locked;
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	bWaitingForEquipSelection = true;
	WaitForEquipDelegate.Broadcast(AbilityType);

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStateFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_State_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection)
	{
		return;
	}

	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))
	{
		return;
	}

	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, SlotTag);

}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StateTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	bWaitingForEquipSelection = false;
	FAuraAbilityInfo LastSlotInfo;

	LastSlotInfo.StateTag = GameplayTags.Abilities_State_Unlocked;
	LastSlotInfo.InputTag = PrevSlotTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if previous slot is a valid slot. Only if equipping an already equipped slot
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StateTag = StateTag;
	Info.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
	
	StopWaitForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	SpellGlobeDeselect();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityState, int32 SpellPoints, bool& bShouldEnableSpendButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (AbilityState.MatchesTagExact(GameplayTags.Abilities_State_Equipped))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
	else if (AbilityState.MatchesTagExact(GameplayTags.Abilities_State_Unlocked))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
	else if (AbilityState.MatchesTagExact(GameplayTags.Abilities_State_Eligible))
	{
		bShouldEnableEquipButton = false;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
	else if (AbilityState.MatchesTagExact(GameplayTags.Abilities_State_Locked))
	{
		bShouldEnableEquipButton = false;
		bShouldEnableSpendButton = false;
	}
}
