// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();

	// Inherited via IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TObjectPtr<UAttributeSet> AttributeSet;
};
