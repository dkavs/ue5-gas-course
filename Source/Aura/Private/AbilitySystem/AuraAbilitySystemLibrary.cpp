// Copyright Giant Rocket Games


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"


UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    // This will be called on widgets which only exist on the local player 
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            return AuraHUD->GetOverlayWidgetControler(WidgetControllerParams);
        }
    }
    
    return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    // This will be called on widgets which only exist on the local player 
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            return AuraHUD->GetAttributeMenuWidgetControler(WidgetControllerParams);
        }
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
    
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
    int CharacterLevel = 1;
    if (CombatInterface)
    {
        CharacterLevel = CombatInterface->GetCharacterLevel();
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

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr)
    {
        return nullptr;
    }

    return AuraGameMode->CharacterClassInfo;
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
    
    return !((bFirstIsPlayer && bSecondIsEnemy) || (bFirstIsEnemy && bSecondIsEnemy));
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
