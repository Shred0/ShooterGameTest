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
class SHOOTERGAME_API UShooterAbilitySystem : public UActorComponent//UObject//AActor
{
	GENERATED_BODY()

	friend class UShooterAbility;

public:	
	// Sets default values for this actor's properties
	UShooterAbilitySystem();

	//method used to link playerstate and user's avatar to ability system
	//static UShooterAbilitySystem* MakeFor(AShooterCharacter* Owner);
	void SetFor(AActor* PlayerState, AShooterCharacter* Owner);

	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	//used to manage passive effects, energy and timed abilities
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	bool bInstanced = false;

	UFUNCTION(BlueprintCallable, Category = "Abilites|Owner")
	AActor* GetActorOwner(); //player state

	UFUNCTION(BlueprintCallable, Category = "Abilites|Avatar")
	AShooterCharacter* GetShooterAvatar(); //player's actor

	//method to add abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool AddAbility(EShooterAbilityID ID);
	UFUNCTION(Server, reliable, WithValidation)
	void ServerAddAbility(EShooterAbilityID ID);

	//method to play active abilities by ID derived from input
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	bool PlayAbility(EShooterAbilityID ID);

	//method to play abilities
	UFUNCTION(BlueprintCallable, Category = "Abilites|Manage")
	void StopAbility(EShooterAbilityID ID);

	//returns the ID of each defined ability
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<EShooterAbilityID> GetAllAbilityIDs();
	//returns the UClass of each defined ability
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<class UClass*> GetAllAbilityClasses();

	//returns the ID of each instanced ability in this ability system
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<EShooterAbilityID> GetEquippedAbilityIDs();
	//returns the reference of each instanced ability in this ability system
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	TArray<class UShooterAbility*> GetEquippedAbilities();

	//returns true if the passed ID coresponds to an instanced ability
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	bool IsAbilityEquipped(EShooterAbilityID ID);
	//returns true if the ability system can instance the ability that coresponds to the passed ID
	UFUNCTION(BlueprintCallable, Category = "Abilites|List")
	bool IsAbilityValid(EShooterAbilityID ID);

	///
	// abilities input
	///

	//method to bind all inputs to corresponding ability IDs
	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void SetKeyBindings(UInputComponent* ShooterInputComponent);

	//called after bind when ability input is pressed
	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void AbilityLocalInputPressed(int32 InputID);
	//called after bind when ability input is pressed, to replicate an ability from server
	UFUNCTION(Server, reliable, WithValidation)
	void AbilityServerInputPressed(EShooterAbilityID ID);

	//called after bind when ability input is released
	UFUNCTION(BlueprintCallable, Category = "Abilites|Bindings")
	void AbilityLocalInputReleased(int32 InputID);
	//called after bind when ability input is released, to replicate an ability from server
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

	///
	//abilities HUD
	///

	UTexture2D* GetHUDAsset();

	UFUNCTION(BlueprintCallable, Category = "Abilites|HUD")
	void DrawAbilityHUD(UCanvas* &Canvas, FVector2D StartPos, float Scale, float Offset, bool DrawFromBottom, bool DrawFromRight, bool IsVerticalArray);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	//TWeakObjectPtr<UShooterAbility> Ability;
	//class UShooterAbility* Ability;
	//UMyObject obj;

	//list of equipped abilities accessible from coresponding ID
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities|Manage|List")
	TMap<EShooterAbilityID, class UShooterAbility*> AbilityMap;

	//shooter actor owner
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Owner")
	AActor* ActorOwner;

	//shooter character avatar
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Avatar")
	AShooterCharacter* ShooterAvatar;

	//true when this ability system is bound to a player state and an avatar
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|Bindings")
	bool IsBound = false;

	///
	//abilities HUD
	///

	/** Texture for new HUD elements. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|HUD")
	UTexture2D* HUDAsset;

	/** FontRenderInfo enabling casting shadow.s */
	FFontRenderInfo ShadowedFont;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|HUD")
	UFont* TextFont;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities|HUD")
	FColor TextColor;

	UWorld* World;

private:

	//reference to all ability classes
	TMap<EShooterAbilityID, UClass*> AbilityClassMap;
};
