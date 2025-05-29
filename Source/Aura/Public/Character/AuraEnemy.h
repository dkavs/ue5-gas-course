// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/InteractableInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IInteractableInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	// Inherited via IInteractableInterface
	virtual void Highlight() override;
	virtual void UnHighlight() override;

	// Inherited via ICombatInterface
	virtual int32 GetCharacterLevel() override;
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribues")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribues")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	// TODO: THis should be done in GAS!
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float DeathLifeSpan = 5.f;

	virtual void Die() override;
	
protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 CharacterLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> WidgetComponent;

};
