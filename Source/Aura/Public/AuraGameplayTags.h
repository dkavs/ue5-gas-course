// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * Aura Gameplay Tags
 * Singleton containing native gameplay tags
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CritChance;
	FGameplayTag Attributes_Secondary_CritDamage;
	FGameplayTag Attributes_Secondary_CritResist;
	FGameplayTag Attributes_Secondary_HealthRegen;
	FGameplayTag Attributes_Secondary_ManaRegen;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Physical;

	FGameplayTag Attributes_Meta_Experience;


	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive1;
	FGameplayTag InputTag_Passive2;

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Physical;

	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Shock;
	FGameplayTag Debuff_Arcane;
	FGameplayTag Debuff_Physical;

	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;

	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_HitReact;

	FGameplayTag Abilities_State_Locked;
	FGameplayTag Abilities_State_Eligible;
	FGameplayTag Abilities_State_Unlocked;
	FGameplayTag Abilities_State_Equipped;

	FGameplayTag Abilities_Type_Active;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBolt;

	FGameplayTag Abilities_Lightning_Electrocute;
	FGameplayTag Cooldown_Lightning_Electrocute;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_Hand_L;
	FGameplayTag CombatSocket_Hand_R;
	FGameplayTag CombatSocket_Tail;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;


	

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuff;

	FGameplayTag Effects_HitReact;

protected:
private:
	static FAuraGameplayTags GameplayTags;
};