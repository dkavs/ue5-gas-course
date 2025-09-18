// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;
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
	virtual int32 GetCharacterLevel_Implementation() override;
	// End Combat interface

	// Player interface
	virtual void AddToExperience_Implementation(int32 Amount) override;
	virtual void AddToPlayerLevel_Implementation(int32 Amount) override;
	virtual void AddToAttributePoints_Implementation(int32 Amount) override;
	virtual void AddToSpellPoints_Implementation(int32 Amount) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetExperience_Implementation() const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual int32 FindLevelForExperience_Implementation(int32 Amount) const override;
	// End Player interface

	UPROPERTY(BlueprintReadonly, VisibleAnywhere)
		TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopdownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	virtual void InitAbilityActorInfo() override;

private:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
