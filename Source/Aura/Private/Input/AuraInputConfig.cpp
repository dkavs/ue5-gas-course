// Copyright Giant Rocket Games


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::GetActionForTag(const FGameplayTag& InputTag, bool bLogIfMissing) const
{
	for (const FAuraInputAction& InputAction : AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputTag == InputTag)
		{
			return InputAction.InputAction;
		}
	}

	if (bLogIfMissing)
	{
		UE_LOG(LogTemp, Error, TEXT("AuraInputConfig::GetActionForTag missing tag %s on input config %s"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
