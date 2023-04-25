// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "GameFramework/Actor.h"
//#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"
//#include "Player/ShooterCharacter.h"
//#include "UObject/NoExportTypes.h"
#include "ShooterAbilitySystem.generated.h"

/*UENUM(BlueprintType)
enum class EShooterAbilityID : uint8
{
	None UMETA(DisplayName = "ShooterAbility"),
	Teleport UMETA(DisplayName = "ShooterAbilityTeleport")
};*/

UCLASS()
class SHOOTERGAME_API AShooterAbilitySystem : public AActor
{
	GENERATED_BODY()

	friend class UShooterAbility;

public:	
	// Sets default values for this actor's properties
	AShooterAbilitySystem();

	static AShooterAbilitySystem* MakeFor(AShooterCharacter* Owner);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Category = "Abilites|Owner")
	AShooterCharacter* GetShooterOwner();

	//method to add abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool AddAbility(EShooterAbilityID ID);

	//method to play abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool PlayAbility(EShooterAbilityID ID);

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<EShooterAbilityID> GetAllAbilities();

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<EShooterAbilityID> GetEquippedAbilities();

	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	bool IsAbilityEquipped(EShooterAbilityID ID);
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	bool IsAbilityValid(EShooterAbilityID ID);

	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void SetKeyBindings(UInputComponent* ShooterInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void AbilityLocalInputPressed(int32 InputID);
	UFUNCTION(Server, reliable, WithValidation)
	void AbilityServerInputPressed(EShooterAbilityID ID);

	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void AbilityLocalInputReleased(int32 InputID);
	UFUNCTION(Server, reliable, WithValidation)
	void AbilityServerInputReleased(EShooterAbilityID ID);

	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void LocalInputConfirm();
	UFUNCTION(Server, reliable, WithValidation)
	void ServerInputConfirm();

	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void LocalInputCancel();
	UFUNCTION(Server, reliable, WithValidation)
	void ServerInputCancel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	//TWeakObjectPtr<UShooterAbility> Ability;
	//class UShooterAbility* Ability;
	//UMyObject obj;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities|Manage|List")
	TMap<EShooterAbilityID, class UShooterAbility*> AbilityMap;

	//shooter character owner
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Owner")
	AShooterCharacter* ShooterOwner;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Bindings")
	bool IsBound = false;

	UWorld* World;

private:

	//reference to all ability classes
	TMap<EShooterAbilityID, UClass*> AbilityClassMap;
};
