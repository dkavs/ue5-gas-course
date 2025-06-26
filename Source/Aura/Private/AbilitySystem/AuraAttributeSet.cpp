// Copyright Giant Rocket Games


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	//InitHealth(100.f);
	//InitMana(50.f);

	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();

	TagsToAttributes.Add(Tags.Attributes_Primary_Strength, UAuraAttributeSet::GetStrengthAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Intelligence, UAuraAttributeSet::GetIntelligenceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Resilience, UAuraAttributeSet::GetResilienceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Primary_Vigor, UAuraAttributeSet::GetVigorAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_Armor, UAuraAttributeSet::GetArmorAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_ArmorPenetration, UAuraAttributeSet::GetArmorPenetrationAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_BlockChance, UAuraAttributeSet::GetBlockChanceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CritChance, UAuraAttributeSet::GetCritChanceAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CritDamage, UAuraAttributeSet::GetCritDamageAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_CritResist, UAuraAttributeSet::GetCritResistAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_HealthRegen, UAuraAttributeSet::GetHealthRegenAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_ManaRegen, UAuraAttributeSet::GetManaRegenAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_MaxHealth, UAuraAttributeSet::GetMaxHealthAttribute);
	TagsToAttributes.Add(Tags.Attributes_Secondary_MaxMana, UAuraAttributeSet::GetMaxManaAttribute);

	TagsToAttributes.Add(Tags.Attributes_Resistance_Arcane, UAuraAttributeSet::GetArcaneResistAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Fire, UAuraAttributeSet::GetFireResistAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Lightning, UAuraAttributeSet::GetLightningResistAttribute);
	TagsToAttributes.Add(Tags.Attributes_Resistance_Physical, UAuraAttributeSet::GetPhysicalResistAttribute);

}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);


	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResist, COND_None, REPNOTIFY_Always);


	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Allows changing the attribute BEFORE the attributes are changed
	// Can be useful for clamping, not recommended to do any real logic here
	// Future logic may modify the values and need to be re-clamped
	// https://github.com/tranek/GASDocumentation?tab=readme-ov-file#445-preattributechange

	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Executes after a GameplayEffecet has executed
	// Can get info about the source, target, effect spec, etc
	// https://github.com/tranek/GASDocumentation?tab=readme-ov-file#446-postgameplayeffectexecute
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s. Health %f"), *EffectProperties.TargetAvatarActor->GetName(), GetHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetHealth(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}
	

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncoming = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalIncoming > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncoming;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor);

				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				EffectProperties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			const bool bBlocked = UAuraAbilitySystemLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
			const bool bCritical = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
			ShowFloatingText(EffectProperties, LocalIncoming, bBlocked, bCritical);
		}
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	UAbilitySystemComponent* SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	Props.SourceASC = SourceASC;

	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC
		->AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		AController* SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			Props.SourceAvatarActor = SourceAvatarActor;
			if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
			}
			Props.SourceController = SourceController;
		}

		if (SourceController != nullptr)
		{
			Props.SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
		}
	}


	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetAvatarActor = TargetAvatarActor;
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, float Damage, bool bIsBlocked, bool bIsCrit) const
{
	if (EffectProperties.SourceCharacter != EffectProperties.TargetCharacter)
	{
		AAuraPlayerController* APC = Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(EffectProperties.SourceCharacter, 0));
		if (APC)
		{
			APC->ShowDamageNumber(Damage, EffectProperties.TargetCharacter, bIsBlocked, bIsCrit);
		}
	}
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}
void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}
void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}
void UAuraAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritChance, OldCritChance);
}
void UAuraAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritDamage, OldCritDamage);
}
void UAuraAttributeSet::OnRep_CritResist(const FGameplayAttributeData& OldCritResist) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritResist, OldCritResist);
}
void UAuraAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegen, OldHealthRegen);
}
void UAuraAttributeSet::OnRep_ManaRegen(const FGameplayAttributeData& OldManaRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegen, OldManaRegen);
}

void UAuraAttributeSet::OnRep_FireResist(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResist, OldValue);
}

void UAuraAttributeSet::OnRep_ArcaneResist(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResist, OldValue);
}

void UAuraAttributeSet::OnRep_LightningResist(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResist, OldValue);
}

void UAuraAttributeSet::OnRep_PhysicalResist(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResist, OldValue);
}

