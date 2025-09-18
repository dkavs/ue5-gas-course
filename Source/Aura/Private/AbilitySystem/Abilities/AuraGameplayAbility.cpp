// Copyright Giant Rocket Games


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 AbilityLevel)
{
    return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsumLoremIpsumLoremIpsum LoremIpsum", AbilityLevel);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 AbilityLevel)
{
    return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</><Default>\nCauses much more damage </>"), AbilityLevel);
}
FString UAuraGameplayAbility::GetLockedDescription(int32 AbilityLevel)
{
    return FString::Printf(TEXT("<Default>Ability Locked Until Level: %d</>"), AbilityLevel);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
    float ManaCost = 0.f;
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (auto Modifier : CostEffect->Modifiers)
        {
            if (Modifier.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
                break;
            }
        }
    }
    return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
    float Cooldown = 0.f;
    if (const UGameplayEffect* CoolEffect = GetCooldownGameplayEffect())
    {
        CoolEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;
}
