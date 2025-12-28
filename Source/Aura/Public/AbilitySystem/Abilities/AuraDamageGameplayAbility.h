// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbilityTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DebuffDuration = 5.f;
};
