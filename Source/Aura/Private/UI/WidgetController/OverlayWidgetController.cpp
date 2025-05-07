// Copyright Giant Rocket Games


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"

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

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda([this](const FGameplayTagContainer AssetTags) {

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
