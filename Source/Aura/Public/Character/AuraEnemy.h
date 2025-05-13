// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/InteractableInterface.h"
#include "AuraEnemy.generated.h"

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
	
protected:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 CharacterLevel = 1;
};
