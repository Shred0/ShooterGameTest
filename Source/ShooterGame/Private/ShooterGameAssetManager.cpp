// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameAssetManager.h"
#include "AbilitySystemGlobals.h"

void UShooterGameAssetManager::StartInitialLoading() {
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}