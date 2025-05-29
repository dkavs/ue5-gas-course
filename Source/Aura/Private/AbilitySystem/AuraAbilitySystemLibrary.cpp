// Copyright Giant Rocket Games


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"


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
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr)
    {
        return;
    }

    AActor* AvatarActor = ASC->GetAvatarActor();

    UCharacterClassInfo* ClassInfo = AuraGameMode->CharacterClassInfo;
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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{

    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr)
    {
        return;
    }

    UCharacterClassInfo* ClassInfo = AuraGameMode->CharacterClassInfo;
    for (const TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        ASC->GiveAbility(AbilitySpec);
    }
}
