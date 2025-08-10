// Copyright Giant Rocket Games


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/AuraPlayerState.h"
#include "Player/LevelInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	const UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AuraAttributes->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributes->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributes->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributes->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	const UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(AttributeSet);
	const UAuraAbilitySystemComponent* AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->ExperienceChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExperienceChanged);
	AuraPlayerState->PlayerLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
	
	
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


	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC->bStartupAbilitiesGiven)
	{
		return;
	}

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, AuraASC](const FGameplayAbilitySpec& AbilitySpec) {
		// TODO: Need a way to get ability tag from Ability Spec
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);

		AbilityInfoDelegate.Broadcast(Info);
	});

	AuraASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnExperienceChanged(int NewExperience) const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelInfo* LevelInfo = AuraPlayerState->GetPlayerLevelInfo();

	/*
	
	LogTemp: Warning: BP_AuraCharacter_C_0 Gained Experience! amount 20.000000
	LogTemp: Warning: EXP Changed. Level 1 / 11. CurrentReq 300. NextLevelReq 900. Delta 600. ExpForThisLevel: -280 Percent: -0.466667
	
	*/

	const int32 Level = AuraPlayerState->GetPlayerLevel();
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
