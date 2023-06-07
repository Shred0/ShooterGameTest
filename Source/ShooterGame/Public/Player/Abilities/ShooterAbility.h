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
class SHOOTERGAME_API UShooterAbility : public UActorComponent//UObject
{
	GENERATED_BODY()
public:
	UShooterAbility();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	//method to create an ability for an ability system given its reference and an ability ID
	static UShooterAbility* MakeFor(UShooterAbilitySystem* SAS, EShooterAbilityID ID);

	//~UShooterAbility();

	///
	//ability getters
	///

	//get reference of ability system wich this ability is bound to
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
	bool GetIsPassiveInCooldown();
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void SetIsPassiveInCooldown(bool bInCoolDown);

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	float GetCooldown();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	bool GetHasPassiveCooldown();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	float GetPassiveCooldown();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown|Timer")
	FTimerHandle GetCooldownTimer();

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown|Timer")
	FTimerHandle GetPassiveCooldownTimer();

	UFUNCTION(BlueprintCallable, Category = "Ability|Duration")
	bool GetIsPlaying();

	UFUNCTION(BlueprintCallable, Category = "Ability|Duration")
	bool HasDuration();

	UFUNCTION(BlueprintCallable, Category = "Ability|Duration")
	float GetDuration();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	bool GetIsEffectActive();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	bool GetHasPassiveEffect();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	bool DoesPassiveReplicate();

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
	//proper method to add energy to ability
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	void AddEnergy(float Energy);
	//proper method to use energy from ability
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	void UseEnergy(float Energy);

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	bool AutoRefills();

	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetRefillRateInTime();
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	float GetDrainRateInTime();

	//function that returns true if the conditions to refill the ability's energy are met
	//cooldown not included
	//needs to be overridden from child classes in order to be effective
	UFUNCTION(BlueprintCallable, Category = "Ability|Energy")
	virtual bool AutoRefillCondition();

	//called when effect is successfully played
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownStart();
	//called at the end of cooldown time
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void CooldownReset();

	//called when effect is successfully played
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void PassiveCooldownStart();
	//called at the end of cooldown time
	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	void PassiveCooldownReset();

	//method for the main ability effect, starts cooldown on effect activation
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	bool PlayEffect();
	//stops main ability effect
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	void StopEffect();

	//method for the main ability effect, starts cooldown on effect activation
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Tick")
	bool PlayTickEffect(float DeltaTime);
	//stops main ability effect
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Tick")
	void StartTickEffect();
	//stops main ability effect
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Tick")
	void StopTickEffect();

	//surrounding ability for the passive ability effect, starts cooldown on effect activation
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	void PlayPassiveEffect(float DeltaTime);
	//UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	//void StopPassiveEffect();
	//proper method to replicate passive effects
	UFUNCTION(Server, Reliable)
	void ServerPlayPassiveEffect(float DeltaTime);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//ability systenm reference
	//needed to replicate to server and instancing abilities
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
	bool IsPassiveInCooldown = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	float AbilityCooldown = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	bool HasPassiveCooldown = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown")
	float PassiveAbilityCooldown = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown|Timer")
	FTimerHandle AbilityCooldownTimer;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Cooldown|Timer")
	FTimerHandle PassiveAbilityCooldownTimer;

	//ability duration
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	bool IsPlaying = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	bool bHasDuration = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	float AbilityDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Duration")
	float AbilityTimeLeft;

	//ability effect
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect")
	bool IsEffectActive = false;

	//ability tick effect
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect|Tick")
	bool HasTickEffect = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect|Tick")
	bool bTickReplicate = false;

	//ability passive
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect|Passive")
	bool HasPassiveEffect = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability|Effect|Passive")
	bool bPassiveReplicate = false;

	///
	// ability HUD
	///

	///
	//FXs
	///

	UFUNCTION(BlueprintCallable, Category = "Abilites|FXs|Sound")
	void PlaySound(USoundCue* Sound, FVector Location);
	UFUNCTION(Server, Reliable)
	void ServerPlaySound(USoundCue* Sound, FVector Location);
	//callable only from server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSound(USoundCue* Sound, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Abilites|FXs|Sound")
	void PlayAudioComponent(UAudioComponent* AudioComponent, float StartTime = 0.f);
	UFUNCTION(Server, Reliable)
	void ServerPlayAudioComponent(UAudioComponent* AudioComponent, float StartTime = 0.f);
	//callable only from server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAudioComponent(UAudioComponent* AudioComponent, float StartTime = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Abilites|FXs|Sound")
	void StopAudioComponent(UAudioComponent* AudioComponent);
	UFUNCTION(Server, Reliable)
	void ServerStopAudioComponent(UAudioComponent* AudioComponent);
	//callable only from server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopAudioComponent(UAudioComponent* AudioComponent);

	UFUNCTION(BlueprintCallable, Category = "Abilites|FXs|Particle")
	void PlayParticle(UParticleSystem* FX, FVector Location, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerPlayParticle(UParticleSystem* FX, FVector Location, FRotator Rotation);
	//callable only from server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastParticle(UParticleSystem* FX, FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Abilites|FXs|Visibility")
	void SetActorVisibility(AActor* Actor, bool bVisible);
	UFUNCTION(Server, Reliable)
	void ServerSetActorVisibility(AActor* Actor, bool bVisible);
	//callable only from server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastActorVisibility(AActor* Actor, bool bVisible);

	///
	//ability functionality
	///

	//energy is refilled and drained from ability system every tick

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

	//world reference based off avatar's world
	//UWorld* World;

private:

	//effects

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Tick")
	virtual int TickEffect(float DeltaTime);

	//called after stopping Effect
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual void AfterEffect();

	//function that returns true if the conditions to play the ability's passive effect are met
	//needs to be overridden from child classes in order to be effective
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	virtual bool PassiveEffectCondition();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Passive")
	virtual void PassiveEffect(float DeltaTime);
};
