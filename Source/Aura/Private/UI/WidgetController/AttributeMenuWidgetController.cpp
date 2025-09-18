// Copyright Giant Rocket Games


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = GetAuraAS();
	check(AttributeInfo);

	for (FAuraAttributeInfo Info : AttributeInfo->AttributeInformation)
	{
		Info.AttributeValue = 0;

		if (AS->TagsToAttributes.Contains(Info.AttributeTag))
		{
			Info.AttributeValue = AS->TagsToAttributes[Info.AttributeTag]().GetNumericValue(AS);
		}
		AttributeInfoDelegate.Broadcast(Info);
	}

	AttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{

	UAuraAttributeSet* AS = GetAuraAS();

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair, AS](const FOnAttributeChangeData& Data)
			{
				FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
				Info.AttributeValue = Pair.Value().GetNumericValue(AS);
				AttributeInfoDelegate.Broadcast(Info);
			});
	}


	GetAuraPS()->AttributePointsChangedDelegate.AddLambda([this](int32 Points) {
		AttributePointsChangedDelegate.Broadcast(Points);
		});
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& GameplayTag)
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	AuraASC->UpgradeAttribute(GameplayTag);
}
