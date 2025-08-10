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
