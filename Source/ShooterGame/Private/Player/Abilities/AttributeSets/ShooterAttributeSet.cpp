// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/Abilities/AttributeSets/ShooterAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UShooterAttributeSet::OnRep_TeleportLocation(const FGameplayAttributeData& OldTeleportLocation) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, TeleportLocation, OldTeleportLocation);
}

void UShooterAttributeSet::OnRep_JetpackFuel(const FGameplayAttributeData& OldJetpackFuel) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, JetpackFuel, OldJetpackFuel);
}

void UShooterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, TeleportLocation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, JetpackFuel, COND_None, REPNOTIFY_Always);
}