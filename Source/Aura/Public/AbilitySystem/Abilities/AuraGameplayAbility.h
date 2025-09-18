// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ScalableFloat.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 AbilityLevel);
	virtual FString GetNextLevelDescription(int32 AbilityLevel);
	static FString GetLockedDescription(int32 AbilityLevel);

protected:
	float GetManaCost(float InLevel = -1) const;
	float GetCooldown(float InLevel = -1) const;
};
