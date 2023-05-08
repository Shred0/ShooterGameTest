// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "ShooterAbilitySystem.h"
#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"

UShooterAbility::UShooterAbility()
{
	//all defaults
	AbilityID = EShooterAbilityID::ShooterAbility;
	AbilityName = FName(TEXT("None"));
	IsInCooldown = false;
	AbilityCooldown = 0.f;
	IsPlaying = false;
	AbilityDuration = 0.f;
	IsActive = false;

	//energy management
	bUsesEnergy = false;
	MaxEnergy = 100.0f;
	Energy = 0.f;

	bAutoRefills = true;
	RefillRateinTime = 25.f;
	DrainRateinTime = 15.f;

	//passive management
	HasPassiveEffect = false;
	bPassiveReplicate = false;
	HasPassiveCooldown = false;
	PassiveAbilityCooldown = 0.f;

	//HUD

	//custom properties
}

UShooterAbility* UShooterAbility::MakeFor(UShooterAbilitySystem* SAS, EShooterAbilityID ID)
{
	UShooterAbility* abilityReference = nullptr;
	if (SAS->IsAbilityValid(ID)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ID is valid");
		//check if ability ID is valid
		UClass* abilityClass = *SAS->AbilityClassMap.Find(ID);
		if (abilityClass) {
			abilityReference = NewObject<UShooterAbility>(SAS, abilityClass);
			abilityReference->AbilitySystem = SAS;
			abilityReference->World = SAS->World;
		}
	}

	return abilityReference;
}

/*UShooterAbility::~UShooterAbility()
{
}*/

UShooterAbilitySystem* UShooterAbility::GetAbilitySystem()
{
	return AbilitySystem;
}

EShooterAbilityID UShooterAbility::GetID()
{
	return AbilityID;
}

FString UShooterAbility::GetName()
{
	return (AbilityName.IsValid()) ? AbilityName.ToString() : "Invalid Name";
}

bool UShooterAbility::GetIsInCooldown()
{
	return IsInCooldown;
}
void UShooterAbility::SetIsInCooldown(bool bInCoolDown)
{
	IsInCooldown = bInCoolDown;
}

bool UShooterAbility::GetIsPassiveInCooldown()
{
	return IsPassiveInCooldown;
}
void UShooterAbility::SetIsPassiveInCooldown(bool bInCoolDown)
{
	IsPassiveInCooldown = bInCoolDown;
}

float UShooterAbility::GetCooldown()
{
	return AbilityCooldown;
}

bool UShooterAbility::GetHasPassiveCooldown()
{
	return HasPassiveCooldown;
}

float UShooterAbility::GetPassiveCooldown()
{
	return PassiveAbilityCooldown;
}

FTimerHandle UShooterAbility::GetCooldownTimer()
{
	return AbilityCooldownTimer;
}

FTimerHandle UShooterAbility::GetPassiveCooldownTimer()
{
	return PassiveAbilityCooldownTimer;
}

bool UShooterAbility::GetIsPlaying()
{
	return IsPlaying;
}

float UShooterAbility::GetDuration()
{
	return AbilityDuration;
}

bool UShooterAbility::GetIsActive()
{
	return IsActive;
}

bool UShooterAbility::GetHasPassiveEffect()
{
	return HasPassiveEffect;
}

bool UShooterAbility::DoesPassiveReplicate()
{
	return bPassiveReplicate;
}

bool UShooterAbility::UsesEnergy()
{
	return bUsesEnergy;
}

float UShooterAbility::GetMaxEnergy()
{
	return MaxEnergy;
}

float UShooterAbility::GetEnergy()
{
	return Energy;
}

void UShooterAbility::SetEnergy(float Energy)
{
	//energy can't be lower than zero and higher than max
	this->Energy = FMath::Clamp(Energy, 0.f, MaxEnergy);
}

void UShooterAbility::AddEnergy(float Energy)
{
	//cant add more energy than the remaining amount of energy to reach max
	this->Energy += FMath::Clamp(Energy, 0.f, MaxEnergy - this->Energy);
}

void UShooterAbility::UseEnergy(float Energy)
{
	//can't use more energy than the current amount
	this->Energy -= FMath::Clamp(Energy, 0.f, this->Energy);
}

bool UShooterAbility::AutoRefills()
{
	return bAutoRefills;
}

float UShooterAbility::GetRefillRateInTime()
{
	return RefillRateinTime;
}

float UShooterAbility::GetDrainRateInTime()
{
	return DrainRateinTime;
}

bool UShooterAbility::AutoRefillCondition()
{
	return true;
}

void UShooterAbility::CooldownStart()
{
	if (World) {
		World->GetTimerManager().SetTimer(AbilityCooldownTimer, this, &UShooterAbility::CooldownReset, AbilityCooldown, false);
		IsInCooldown = true;
	}
}
void UShooterAbility::CooldownReset()
{
	IsInCooldown = false;
}

void UShooterAbility::PassiveCooldownStart()
{
	if (World) {
		World->GetTimerManager().SetTimer(PassiveAbilityCooldownTimer, this, &UShooterAbility::PassiveCooldownReset, PassiveAbilityCooldown, false);
		IsPassiveInCooldown = true;
	}
}
void UShooterAbility::PassiveCooldownReset()
{
	IsPassiveInCooldown = false;
}

bool UShooterAbility::PlayEffect()
{
	if (World->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Ability in Client");
	}

	//generic error case
	int result = -1;

	//if is not in cooldown and can drain energy
	if (!IsInCooldown || (UsesEnergy() && DrainRateinTime >= Energy)) {
		if (UsesEnergy()) {
			IsPlaying = true;
		}
		result = Effect();
	}

	bool successfullyPlayed = (result == 0) ? true : false;

	if (successfullyPlayed) {
		CooldownStart();
	}

	return successfullyPlayed;
}

void UShooterAbility::StopEffect()
{
	if (World->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Ability in Client");
	}

	AfterEffect();

	IsPlaying = false;
}

void UShooterAbility::PlayPassiveEffect()
{
	if (GetHasPassiveEffect() && PassiveEffectCondition() && !GetIsPassiveInCooldown()) {
		PassiveEffect();
		if (GetHasPassiveCooldown()) {
			PassiveCooldownStart();
		}
	}
}

int UShooterAbility::Effect()
{
	return 0;
}

void UShooterAbility::AfterEffect()
{
}

bool UShooterAbility::PassiveEffectCondition()
{
	return true;
}

void UShooterAbility::PassiveEffect()
{
}
