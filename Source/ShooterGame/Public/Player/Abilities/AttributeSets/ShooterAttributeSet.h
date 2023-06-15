// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ShooterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Teleport", ReplicatedUsing = OnRep_TeleportLocation)
	FGameplayAttributeData TeleportLocation;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, TeleportLocation)

	UPROPERTY(BlueprintReadOnly, Category = "Jetpack", ReplicatedUsing = OnRep_JetpackFuel)
	FGameplayAttributeData JetpackFuel;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, JetpackFuel)

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_TeleportLocation(const FGameplayAttributeData& OldTeleportLocation);

	UFUNCTION()
	virtual void OnRep_JetpackFuel(const FGameplayAttributeData& OldJetpackFuel);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
