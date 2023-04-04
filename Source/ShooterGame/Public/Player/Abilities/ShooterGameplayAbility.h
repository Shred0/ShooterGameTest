// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "ShooterGame\Public\ShooterGame.h"
#include "ShooterGameplayAbility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ShooterAbilityID : uint8
{
	None UMETA(DisplayName = "None"),
	Teleport UMETA(DisplayName = "Teleport")
};

UCLASS()
class SHOOTERGAME_API UShooterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UShooterGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
		ShooterAbilityID AbilityID = ShooterAbilityID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
		ShooterAbilityID AbilityInputID = ShooterAbilityID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
