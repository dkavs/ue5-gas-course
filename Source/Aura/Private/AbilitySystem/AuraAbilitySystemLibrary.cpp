// Copyright Giant Rocket Games


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Game/AuraGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"


bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
    // This will be called on widgets which only exist on the local player 
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();
            OutWCParams.PlayerState = PS;
            OutWCParams.AbilitySystemComponent = ASC;
            OutWCParams.AttributeSet = AS;
            OutWCParams.PlayerController = PC;

            OutAuraHUD = AuraHUD;
            return true;

        }
    }
    return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD);
    if (bSuccessfulParams)
    {
        return AuraHUD->GetOverlayWidgetController(WCParams);
    }
    
    return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD);
    if (bSuccessfulParams)
    {
        return AuraHUD->GetAttributeMenuWidgetController(WCParams);
    }

    return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD);
    if (bSuccessfulParams)
    {
        return AuraHUD->GetSpellMenuWidgetController(WCParams);
    }

    return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
    AActor* AvatarActor = ASC->GetAvatarActor();

    UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
    FCharacterClassDefaultInfo DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

    FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
    PrimaryContextHandle.AddSourceObject(AvatarActor);

    const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(DefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());

    FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
    SecondaryContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());


    FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
    VitalContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
    UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (ClassInfo == nullptr)
    {
        return;
    }
    
    int CharacterLevel = 1;
    if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
    {
        CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor());
    }



    for (const TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CharacterLevel);
        ASC->GiveAbility(AbilitySpec);
    }

    FCharacterClassDefaultInfo DefaultClassInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
    for (const TSubclassOf<UGameplayAbility> AbilityClass : DefaultClassInfo.StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CharacterLevel);
        ASC->GiveAbility(AbilitySpec);
    }
}

int32 UAuraAbilitySystemLibrary::GetExpForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{

    UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (ClassInfo == nullptr)
    {
        return 0;
    }

    FScalableFloat ScalableExp = ClassInfo->GetClassDefaultInfo(CharacterClass).ExpReward;
    const float ExpReward = ScalableExp.GetValueAtLevel(CharacterLevel);
    return static_cast<int32>(ExpReward);
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr)
    {
        return nullptr;
    }

    return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr)
    {
        return nullptr;
    }

    return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsBlockedHit();
    }

    return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsCriticalHit();
    }

    return false;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector SphereOrigin)
{
    FCollisionQueryParams SphereParams;
    SphereParams.AddIgnoredActors(ActorsToIgnore);


    // query scene to see what we hit
    TArray<FOverlapResult> Overlaps;
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
    }

    for (const FOverlapResult& Overlap : Overlaps)
    {
        if (Overlap.GetActor()->Implements<UCombatInterface>())
        {
            if (!ICombatInterface::Execute_IsDead(Overlap.GetActor()))
            {
                OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
            }
        }
    }
}

bool UAuraAbilitySystemLibrary::IsNotFriendly(AActor* FirstActor, AActor* SecondActor)
{
    const bool bFirstIsPlayer = FirstActor->ActorHasTag(FName("Player"));
    const bool bSecondIsPlayer = SecondActor->ActorHasTag(FName("Player"));

    const bool bFirstIsEnemy = FirstActor->ActorHasTag(FName("Enemy"));
    const bool bSecondIsEnemy = SecondActor->ActorHasTag(FName("Enemy"));
    
    return (bFirstIsPlayer && bSecondIsEnemy) || (bFirstIsEnemy && bSecondIsPlayer) || (!bSecondIsPlayer && !bSecondIsEnemy);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
    const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
    EffectContextHandle.AddSourceObject(SourceAvatarActor);
    
    FGameplayEffectSpecHandle DamageSpecHandle = DamageEffectParams.SourceASC->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);
   
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

    DamageEffectParams.TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data);

    return EffectContextHandle;
}



void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlocked)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->SetBlockedHit(true);
    }
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsCritical)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->SetCriticalHit(true);
    }
}
