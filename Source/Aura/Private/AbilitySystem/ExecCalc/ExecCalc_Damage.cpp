// Copyright Giant Rocket Games


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"


struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritResist);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResist);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResist);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResist);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResist);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritResist, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResist, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResist, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResist, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResist, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritDamage, Source, false);

		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CritResist, CritResistDef);

		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, ArcaneResistDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Fire, FireResistDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Lightning, LightningResistDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Physical, PhysicalResistDef);


		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CritChance, CritChanceDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CritDamage, CritDamageDef);
	}

};

static const AuraDamageStatics DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritResistDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = SourceASC ? TargetASC->GetAvatarActor() : nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceActor);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetActor);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;


	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceActor);
	const FRealCurve* ArmorPenCurve = CharacterClassInfo->DamageCoefficientsTable->FindCurve(FName("ArmorPenetration"), FString());
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCoefficientsTable->FindCurve(FName("EffectiveArmor"), FString());
	const FRealCurve* CritResistCurve = CharacterClassInfo->DamageCoefficientsTable->FindCurve(FName("CritResist"), FString());
	const int PlayerLevel = SourceCombatInterface->GetCharacterLevel();
	const int TargetLevel = TargetCombatInterface->GetCharacterLevel();

	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	
	// Get damage set by caller magnetude 
	float Damage = 0.f;

	for (const auto& DamageTypeMapping : FAuraGameplayTags::Get().DamageTypesToResistances)
	{

		const FGameplayTag DamageType = DamageTypeMapping.Key;
		const FGameplayTag ResistType = DamageTypeMapping.Value;
		checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistType), TEXT("TagsToCaptureDefs doesn't contain tag %s in ExecCalc_Damage"), *ResistType.ToString());

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistType];
		
		float DamageForType = Spec.GetSetByCallerMagnitude(DamageType);
		float ResistanceToType = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceToType);
		ResistanceToType = FMath::Clamp<float>(ResistanceToType, -1.f, 1.f);

		DamageForType *= (1.f - ResistanceToType);

		Damage += DamageForType;
	}

	// If we've blocked - cut the source damage in half
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	if (FMath::FRand() < TargetBlockChance)
	{
		Damage *= 0.5f;
		UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, true);
	}


	// Crit resist reduces the CHANCE of being crit. Does not mitigate damage
	float TargetCritResist = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritResistDef, EvaluationParameters, TargetCritResist);

	float SourceCritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluationParameters, SourceCritChance);
	if (SourceCritChance > 0 && FMath::FRand() < SourceCritChance)
	{
		float SourceCritDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluationParameters, SourceCritDamage);
		SourceCritDamage -= CritResistCurve->Eval(TargetLevel);
		SourceCritDamage = FMath::Max<float>(SourceCritDamage, 0.f) + 1.f;
		Damage *= SourceCritDamage;
		UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, true);
	}

	// Armor Pen ignores a percentage of the target's armor
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	
	float SourceArmorPen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPen);
	SourceArmorPen = FMath::Max<float>(SourceArmorPen, 0.f);


	
	
	const float ArmorPenCoef = ArmorPenCurve->Eval(PlayerLevel);
	const float EffectiveArmorCoef = EffectiveArmorCurve->Eval(PlayerLevel);
	// Note: In the course he uses an armor penetration value and uses a formula to make 400 armor pen = 100% penetration.
	// I'm going to keep it simple and be a flat 0-1%
	const float EffectiveArmor = TargetArmor * (1.f - (SourceArmorPen * ArmorPenCoef));

	// Current formula means every 3 armor reduces damage by 1%
	// Probably need a better formula but good enough for now
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoef) / 100.f;


	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
