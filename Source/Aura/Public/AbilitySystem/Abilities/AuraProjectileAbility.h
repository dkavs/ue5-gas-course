// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraProjectileAbility.generated.h"

class AAuraProjectile;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileAbility : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& TargetLocation);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
};
