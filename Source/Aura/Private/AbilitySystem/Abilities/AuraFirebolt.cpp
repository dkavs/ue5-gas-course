// Copyright Giant Rocket Games

#include "AbilitySystem/Abilities/AuraFirebolt.h"
#include "AuraGameplayTags.h"


FString UAuraFirebolt::GetDescription(int32 AbilityLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(AbilityLevel);
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = GetCooldown(AbilityLevel);

	if (AbilityLevel == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Launches a bolt of fire, exploding on impact and dealing </><Damage>%d</><Default> fire damage with a chance to burn</>"
			"\n\n"),
			AbilityLevel,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	else
	{
		int32 CurProjectiles = FMath::Min(AbilityLevel, Projectiles);
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d</><Default> fire damage with a chance to burn</>"
			"\n\n"),
			AbilityLevel,
			ManaCost,
			Cooldown,
			CurProjectiles,
			ScaledDamage);
	}
}

FString UAuraFirebolt::GetNextLevelDescription(int32 AbilityLevel)	
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(AbilityLevel);
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = GetCooldown(AbilityLevel);
	int32 CurProjectiles = FMath::Min(AbilityLevel, Projectiles);
	return FString::Printf(TEXT("<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d</><Default> fire damage with a chance to burn</>"
		"\n\n"),
		AbilityLevel,
		ManaCost,
		Cooldown,
		CurProjectiles,
		ScaledDamage);
}