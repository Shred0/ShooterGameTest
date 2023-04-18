// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterAbilitySystem.h"
//#include "ShooterAbility.h"
#include "Player/Abilities/ShooterAbility.h"

UShooterAbility::UShooterAbility()
{
	AbilitySystem = nullptr;
	AbilityID = UShooterAbilityID::None;
	AbilityName = FName(TEXT("None"));
	IsInCooldown = false;
	AbilityCooldown = 0.f;
	IsPlaying = false;
	AbilityDuration = 0.f;
	IsActive = false;
}
UShooterAbility::UShooterAbility(AShooterAbilitySystem* SAS)
{
	AbilitySystem = SAS;
	AbilityID = UShooterAbilityID::None;
	AbilityName = FName(TEXT("None"));
	IsInCooldown = false;
	AbilityCooldown = 0.f;
	IsPlaying = false;
	AbilityDuration = 0.f;
	IsActive = false;
}

/*UShooterAbility::~UShooterAbility()
{
}*/

AShooterAbilitySystem* UShooterAbility::GetAbilitySystem()
{
	return AbilitySystem;
}

UShooterAbilityID UShooterAbility::GetID()
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
	GetWorld()->GetTimerManager().SetTimer(AbilityCooldownTimer, this, &UShooterAbility::CooldownReset, AbilityCooldown, false);
	IsInCooldown = true;
}
void UShooterAbility::CooldownReset()
{
	IsInCooldown = false;
}

int UShooterAbility::Effect()
{
	return 0;
}

bool UShooterAbility::PlayEffect()
{
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
