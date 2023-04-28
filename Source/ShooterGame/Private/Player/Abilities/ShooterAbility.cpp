// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "ShooterAbilitySystem.h"
#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"

UShooterAbility::UShooterAbility()
{
	//AbilitySystem = nullptr;
	AbilityID = EShooterAbilityID::ShooterAbility;
	AbilityName = FName(TEXT("None"));
	IsInCooldown = false;
	AbilityCooldown = 0.f;
	IsPlaying = false;
	AbilityDuration = 0.f;
	IsActive = false;
}

UShooterAbility* UShooterAbility::MakeFor(UShooterAbilitySystem* SAS, EShooterAbilityID ID)
{
	UShooterAbility* abilityReference = nullptr;
	if (SAS->IsAbilityValid(ID)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ID is valid");
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

float UShooterAbility::GetCooldown()
{
	return AbilityCooldown;
}

FTimerHandle UShooterAbility::GetCooldownTimer()
{
	return AbilityCooldownTimer;
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

	if (!IsInCooldown) {
		result = Effect();
	}

	bool successfullyPlayed = (result == 0) ? true : false;

	if (successfullyPlayed) {
		CooldownStart();
	}

	return successfullyPlayed;
}

int UShooterAbility::Effect()
{
	return 0;
}
