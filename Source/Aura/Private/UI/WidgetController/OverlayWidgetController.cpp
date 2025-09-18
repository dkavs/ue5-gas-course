// Copyright Giant Rocket Games


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AttributeSet.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Player/AuraPlayerState.h"
#include "Player/LevelInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributes = GetAuraAS();
	
	OnHealthChanged.Broadcast(AuraAttributes->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributes->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributes->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributes->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributes = GetAuraAS();
	
	AAuraPlayerState* AuraPS = GetAuraPS();
	AuraPS->ExperienceChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExperienceChanged);
	AuraPS->PlayerLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributes->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributes->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributes->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
			OnManaChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributes->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});


	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		AuraASC->AbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (AuraASC->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		AuraASC->EffectAssetTags.AddLambda([this](const FGameplayTagContainer AssetTags) {
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
	}

	
}

void UOverlayWidgetController::OnExperienceChanged(int NewExperience)
{
	AAuraPlayerState* AuraPS = GetAuraPS();
	const ULevelInfo* LevelInfo = AuraPS->GetPlayerLevelInfo();

	/*
	
	LogTemp: Warning: BP_AuraCharacter_C_0 Gained Experience! amount 20.000000
	LogTemp: Warning: EXP Changed. Level 1 / 11. CurrentReq 300. NextLevelReq 900. Delta 600. ExpForThisLevel: -280 Percent: -0.466667
	
	*/

	const int32 Level = AuraPS->GetPlayerLevel();
	const int32 MaxLevel = LevelInfo->LevelInfo.Num();

	const int32 CurrentLevelReq = LevelInfo->GetExpRequiredForLevel(Level);
	const int32 NextLevelReq = LevelInfo->GetExpRequiredForLevel(Level + 1);
	const int32 Delta = NextLevelReq - CurrentLevelReq;

	const int32 ExpForThisLevel = NewExperience - CurrentLevelReq;


	float Percent = 1.f;
	if (Delta > 0)
	{
		Percent = static_cast<float>(ExpForThisLevel) / static_cast<float>(Delta);
	}

	UE_LOG(LogTemp, Warning, TEXT("EXP Changed. Level %d / %d. CurrentReq %d. NextLevelReq %d. Delta %d. ExpForThisLevel: %d Percent: %f"), Level, MaxLevel, CurrentLevelReq, NextLevelReq, Delta, ExpForThisLevel, Percent);


	/*
	* 
		LogTemp: Warning: BP_AuraCharacter_C_0 Gained Experience! amount 20.000000 
		LogBlueprintUserMessages: [WBP_XPBar] Server: xp bar percent: -0.466667

	*/
	PlayerExpPercentChanged.Broadcast(Percent);
}

void UOverlayWidgetController::OnLevelChanged(int NewLevel) const
{
	PlayerLevelChanged.Broadcast(NewLevel);
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StateTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag) const
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
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
}
