// Copyright Giant Rocket Games


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Player/LevelInfo.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AAuraPlayerState, PlayerLevel);
	DOREPLIFETIME(AAuraPlayerState, PlayerExperience);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::SetPlayerLevel(int32 Amount)
{
	PlayerLevel = Amount;
	PlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AAuraPlayerState::AddToPlayerLevel(int32 Amount)
{
	PlayerLevel += Amount;
	PlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AAuraPlayerState::SetExperience(int32 Amount)
{
	PlayerExperience = Amount;
	ExperienceChangedDelegate.Broadcast(PlayerExperience);
}

void AAuraPlayerState::AddToExperience(int32 Amount)
{
	PlayerExperience += Amount;
	ExperienceChangedDelegate.Broadcast(PlayerExperience);
}

void AAuraPlayerState::ChangeAttributePoints(int32 Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("ChangeAttributePoints:: Old: %d  New: %d"), AttributePoints, AttributePoints + Amount);
	AttributePoints += Amount;
	AttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::ChangeSpellPoints(int32 Amount)
{
	SpellPoints += Amount;
	SpellPointsChangedDelegate.Broadcast(SpellPoints);
}


void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	PlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AAuraPlayerState::OnRep_Experience(int32 OldExperience)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep EXP. Old: %d  New: %d"), OldExperience, PlayerExperience);
	//ExperienceChangedDelegate.Broadcast(PlayerExperience);
	ExperienceChangedDelegate.Broadcast(PlayerExperience);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldPoints)
{
	AttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldPoints)
{
	SpellPointsChangedDelegate.Broadcast(SpellPoints);
}
