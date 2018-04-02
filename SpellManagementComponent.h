// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Public/GenericQuadTree.h"
#include "Components/ActorComponent.h"

#include "SpellManagementComponent.generated.h"

UENUM(BlueprintType)
enum class ESpellNode : uint8
{
	LIGHT 	UMETA(DisplayName = "Light"),
	AIR 	UMETA(DisplayName = "Air"),
	WATER	UMETA(DisplayName = "Water"),
	DARK   UMETA(DisplayName = "Dark"),
	EARTH    UMETA(DisplayName = "Earth"),
	FIRE    UMETA(DisplayName = "Fire"),
	LIGHT2    UMETA(DisplayName = "Light2"),
	AIR2  UMETA(DisplayName = "Air2"),
	WATER2  UMETA(DisplayName = "Water2"),
	DARK2  UMETA(DisplayName = "Dark2"),
	EARTH2   UMETA(DisplayName = "Earth2"),
	FIRE2   UMETA(DisplayName = "Fire2"),
	LIFE  UMETA(DisplayName = "Life"),
	ICE    UMETA(DisplayName = "Ice"),
	POISON   UMETA(DisplayName = "Poison"),
	DEATH  UMETA(DisplayName = "Death"),
	METAL   UMETA(DisplayName = "Metal"),
	ENERGY  UMETA(DisplayName = "Energy"),
	INVALID UMETA(DisplayName = "Invalid")
};

USTRUCT(BlueprintType)
struct FSpellComponent
{
	GENERATED_USTRUCT_BODY()

	FSpellComponent()
	{
		SpellNode = ESpellNode::INVALID;
		Quality = 0.0f;
	}

	UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	ESpellNode SpellNode;

	UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	float Quality;
};


USTRUCT(BlueprintType)
struct FSpell {
	GENERATED_USTRUCT_BODY()

	//FSpell()
	//{
	//	Damage = 0.0f;
	//}

	UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	int32 ID;

	UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	TArray<ESpellNode> SpellNodes;

	UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	FText Name;

	//UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	//TSubclassOf<class AExArcanisProjectile>	Projectile;
	
	//UPROPERTY(BlueprintReadWrite, Category = "ExArcanis")
	//float Damage;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellCastFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSpellCastNodeReached, ESpellNode, SpellNode, FVector2D, Position, float, Quality);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXARCANIS_API USpellManagementComponent : public UActorComponent
{
	GENERATED_BODY()
	FBox2D BoundingBox;
	TQuadTree<ESpellNode> QuadTree;

public:	
	// Sets default values for this component's properties
	USpellManagementComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "ExArcanis")
	FSpellCastFailed SpellCastFailed;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "ExArcanis")
	FSpellCastNodeReached SpellCastNodeReached;

	UFUNCTION(BlueprintCallable, Category = "ExArcanis")
	void Update(const APlayerController * PlayerController);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExArcanis")
	FVector2D SpellNodeExtend = FVector2D(0.025f, 0.025f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExArcanis")
	float SpellFailMargin = 0.7f;
	
	UFUNCTION(BlueprintCallable, Category = "ExArcanis")
	void GetSpell(FSpell & Spell, float & Quality, bool & Valid);

	UFUNCTION(BlueprintCallable, Category = "ExArcanis")
	void AddSpell(FSpell const & Spell);

	UFUNCTION(BlueprintCallable, Category = "ExArcanis")
	void Reset();

private:
	TArray<FSpellComponent> CurrentSpellComponents;
	TArray<FBox2D> SpellNodes;
	TMap<uint32, FSpell> Spells;
	
	float CurrentDistance;
	FVector2D LastMousePosition;
};
