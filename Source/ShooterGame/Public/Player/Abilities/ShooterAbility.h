// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
//#include "ShooterAbilitySystem.h"
#include "UObject/NoExportTypes.h"
#include "ShooterAbility.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SHOOTERGAME_API UShooterAbility : public UObject
{
	GENERATED_BODY()
public:
	UShooterAbility();

	static UShooterAbility* MakeFor(UShooterAbilitySystem* SAS, EShooterAbilityID ID);

	//~UShooterAbility();

	///
	//ability getters
	///

	UFUNCTION(BlueprintCallable, Category = "Ability|AbilitySystem")
	UShooterAbilitySystem* GetAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "Ability|ID")
	EShooterAbilityID GetID();

	UFUNCTION(BlueprintCallable, Category = "Ability|Name")
	FString GetName();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	bool GetIsInCooldown();
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void SetIsInCooldown(bool bInCoolDown);

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
	// ability HUD
	///

	/** Texture for new HUD elements. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|HUD")
	UTexture2D* HUDAsset;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|HUD")
	FCanvasIcon AbilityIcon;
	
	///
	//ability functions
	///

	///
	//ability functionality
	///
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	bool UsesEnergy();

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetMaxEnergy();

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetEnergy();
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	void SetEnergy(float Energy);
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	void AddEnergy(float Energy);
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	void UseEnergy(float Energy);

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	bool AutoRefills();

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetRefillRateInTime();
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetDrainRateInTime();

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	virtual bool AutoRefillCondition();

	//called when effect is successfully played
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownStart();
	//called at the end of cooldown time
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownReset();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	bool PlayEffect();
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	void StopEffect();

protected:

	//ability systenm reference
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|AbilitySystem")
	UShooterAbilitySystem* AbilitySystem;
	
	//ability ID
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|ID")
	EShooterAbilityID AbilityID = EShooterAbilityID::ShooterAbility;

	//ability Name
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Name")
	FName AbilityName = FName(TEXT("None"));

	//ability cooldown
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	bool IsInCooldown = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	float AbilityCooldown = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown|Timer")
	FTimerHandle AbilityCooldownTimer;

	//ability duration
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	bool IsPlaying = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	float AbilityDuration = 0.f;

	//ability effect
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect")
	bool IsActive = false;

	///
	// ability HUD
	///

	///
	//ability functionality
	///

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	bool bUsesEnergy = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	float MaxEnergy = 100.f;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	float Energy = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	bool bAutoRefills = true;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	float RefillRateinTime = 25.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Energy")
	float DrainRateinTime = 15.f;

	//world
	UWorld* World;

private:

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual void AfterEffect();
};
