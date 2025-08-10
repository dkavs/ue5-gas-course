// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	// Combat interface
	virtual int32 GetCharacterLevel() override;
	// End Combat interface

	// Player interface
	virtual void AddToExperience_Implementation(int32 Amount) override;
	// End Player interface

protected:
	virtual void InitAbilityActorInfo() override;

};
