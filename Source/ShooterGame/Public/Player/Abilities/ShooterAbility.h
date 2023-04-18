// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterAbilitySystem.h"
#include "UObject/NoExportTypes.h"
#include "ShooterAbility.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbility : public UObject
{
	GENERATED_BODY()
public:
	UShooterAbility();
	UShooterAbility(AShooterAbilitySystem* SAS);

	//~UShooterAbility();

	///
	//ability getters
	///

	UFUNCTION(BlueprintCallable, Category = "Ability|AbilitySystem")
	AShooterAbilitySystem* GetAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "Ability|ID")
	UShooterAbilityID GetID();

	UFUNCTION(BlueprintCallable, Category = "Ability|Name")
	FString GetName();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	bool GetIsInCooldown();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	float GetCooldown();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown|Timer")
	FTimerHandle GetCooldownTimer();

	UFUNCTION(BlueprintCallable, Category = "Ability|Duration")
	bool GetIsPlaying();

	UFUNCTION(BlueprintCallable, Category = "Ability|Duration")
	float GetDuration();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	bool GetIsActive();
	
	///
	//ability functions
	///

	//called when effect is successfully played
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownStart();
	//called at the end of cooldown time
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownReset();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	bool PlayEffect();

protected:

	//ability systenm reference
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|AbilitySystem")
	AShooterAbilitySystem* AbilitySystem;
	
	//ability ID
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|ID")
	UShooterAbilityID AbilityID;

	//ability Name
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Name")
	FName AbilityName;

	//ability cooldown
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	bool IsInCooldown;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	float AbilityCooldown;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown|Timer")
	FTimerHandle AbilityCooldownTimer;

	//ability duration
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	bool IsPlaying;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	float AbilityDuration;

	//ability effect
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect")
	bool IsActive;

	//energy pool

	//owner

	//target
};
