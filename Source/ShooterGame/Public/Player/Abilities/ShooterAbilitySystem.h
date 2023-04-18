// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterGame.h"
#include "GameFramework/Actor.h"
#include "ShooterAbility.h"
#include "Player/Abilities/ShooterAbility.h"
#include "Player/ShooterCharacter.h"
#include "ShooterAbilitySystem.generated.h"

UENUM(BlueprintType)
enum class UShooterAbilityID : uint8
{
	None UMETA(DisplayName = "ShooterAbility"),
	Teleport UMETA(DisplayName = "ShooterAbilityTeleport")
};

UCLASS()
class SHOOTERGAME_API AShooterAbilitySystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterAbilitySystem();
	AShooterAbilitySystem(AShooterCharacter* Owner);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Abilites|Owner")
	AShooterCharacter* GetShooterOwner();

	//method to add abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool AddAbility(UShooterAbilityID ID);

	//method to play abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool PlayAbility(UShooterAbilityID ID);

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<UShooterAbilityID> GetAllAbilities();

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<UShooterAbilityID> GetEquippedAbilities();

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	bool IsAbilityEquipped(UShooterAbilityID ID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	//TWeakObjectPtr<UShooterAbility> Ability;
	//class UShooterAbility* Ability;
	//UMyObject obj;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities|Manage|List")
	TMap<UShooterAbilityID, class UShooterAbility*> AbilityMap;

	//shooter character owner
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Owner")
	AShooterCharacter* ShooterOwner;
};
