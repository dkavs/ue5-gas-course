// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32);

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE int32 GetPlayerLevel() const { return PlayerLevel; }
	void SetPlayerLevel(int32 Amount);
	void AddToPlayerLevel(int32 Amount);
	FOnPlayerStatChanged PlayerLevelChangedDelegate;


	FORCEINLINE int32 GetPlayerExperience() const { return PlayerExperience; }
	void SetExperience(int32 Amount);
	void AddToExperience(int32 Amount);

	//UPROPERTY(BlueprintAssignable, Category = "Progression")
	FOnPlayerStatChanged ExperienceChangedDelegate;

	FORCEINLINE TObjectPtr<ULevelInfo> GetPlayerLevelInfo() const { return PlayerLevelInfo; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Progression")
	TObjectPtr<ULevelInfo> PlayerLevelInfo;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 PlayerLevel = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Experience)
	int32 PlayerExperience = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_Experience(int32 OldExperience);
};
