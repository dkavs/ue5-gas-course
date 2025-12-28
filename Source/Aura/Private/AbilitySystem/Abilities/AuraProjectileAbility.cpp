// Copyright Giant Rocket Games


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraProjectileAbility::SpawnProjectile(const FVector& TargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor->HasAuthority())
	{
		return;
	}

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);

	if (CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);
		FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
		// This was causing issues on Multiplayer where projectiles sometimes were above enemies
		//Rotation.Pitch = 0.f;

		if (bOverridePitch == true)
		{
			Rotation.Pitch = PitchOverride;
		}

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AActor* OwningActor = GetOwningActorFromActorInfo();
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor, Cast<APawn>(OwningActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		Projectile->DamageEffectParams = DamageEffectParams;

		Projectile->FinishSpawning(SpawnTransform);
	}
}


void UAuraProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
