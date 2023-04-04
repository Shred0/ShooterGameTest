// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ShooterGameAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterGameAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	virtual void StartInitialLoading() override;
};
