// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellManagementComponent.h"
#include "ExArcanis.h"
#include "Engine/LocalPlayer.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"


// Sets default values for this component's properties
USpellManagementComponent::USpellManagementComponent() : QuadTree(FBox2D())
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	//SpellNodeExtend = FVector2D(0.01f, 0.01f);
	//SpellFailMargin = 0.7f;
	Reset();

	for (float Theta = 0.0f; Theta < FMath::DegreesToRadians(330); Theta += FMath::DegreesToRadians(60))
	{
		FVector2D Center(FMath::Sin(Theta), -FMath::Cos(Theta));
		SpellNodes.Add(FBox2D(Center * 0.32f - SpellNodeExtend, Center * 0.32f + SpellNodeExtend));
	}

	for (float Theta = 0.0f; Theta < FMath::DegreesToRadians(330); Theta += FMath::DegreesToRadians(60)) {
		FVector2D Center(FMath::Sin(Theta), -FMath::Cos(Theta));
		SpellNodes.Add(FBox2D(Center * 0.63f - SpellNodeExtend, Center * 0.63f + SpellNodeExtend));
	}

	for (float Theta = FMath::DegreesToRadians(30.0f); Theta < FMath::DegreesToRadians(360); Theta += FMath::DegreesToRadians(60)) {
		FVector2D Center(FMath::Sin(Theta), -FMath::Cos(Theta));
		SpellNodes.Add(FBox2D(Center * 0.86f - SpellNodeExtend, Center * 0.86f + SpellNodeExtend));
	}

	BoundingBox = FBox2D(FVector2D(-1.0f, -1.0f), FVector2D(1.0f, 1.0f));
	QuadTree = TQuadTree<ESpellNode>(BoundingBox);
	for (int32 Index = 0; Index < SpellNodes.Num(); ++Index)
	{
		FBox2D & SpellNode = SpellNodes[Index];
		QuadTree.Insert((ESpellNode)Index, SpellNode);
		UE_LOG(LogTemp, Log, TEXT("%s"), *SpellNode.GetCenter().ToString());
	}

	// ...
}


// Called when the game starts
void USpellManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USpellManagementComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void USpellManagementComponent::Update(const APlayerController * PlayerController) 
{	
	if (!PlayerController) return;

	const ULocalPlayer * Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player) return;

	const UGameViewportClient * ViewportClient =
		Cast < UGameViewportClient >(Player->ViewportClient);

	if (!ViewportClient) return;

	FViewport * Viewport = ViewportClient->Viewport;

	if (!Viewport) return;

	float MouseX = float(Viewport->GetMouseX());
	float MouseY = float(Viewport->GetMouseY());
	FIntPoint Size = Viewport->GetSizeXY();
	float HalfSizeX = float(Size.X) / 2.0f;
	float HalfSizeY = float(Size.Y) / 2.0f;
	float Norm = FMath::Min(HalfSizeX, HalfSizeY);

	FVector2D MousePosition((MouseX - HalfSizeX) / Norm, (MouseY - HalfSizeY) / Norm);
	FVector2D Extend(0.01f, 0.01f);

	CurrentDistance += FVector2D::Distance(LastMousePosition, MousePosition);
	if (CurrentDistance > SpellFailMargin)
	{
		SpellCastFailed.Broadcast();
		return;
	}

	LastMousePosition = MousePosition;

	FBox2D QueryBox(MousePosition - Extend, MousePosition + Extend);
	if (!BoundingBox.IsInside(QueryBox)) return;

	TArray<ESpellNode> CurrentSpellNodes;
	QuadTree.GetElements(QueryBox, CurrentSpellNodes);

	if (CurrentSpellNodes.Num() == 0) return;
	
	if (CurrentSpellComponents.Num() > 0 && CurrentSpellComponents.Last().SpellNode == CurrentSpellNodes[0]) return;

	FVector2D LastSpellNodePosition = FVector2D(0.0f, 0.0f);
	if (CurrentSpellComponents.Num() > 0)
	{
		LastSpellNodePosition = SpellNodes[(int32)CurrentSpellComponents.Last().SpellNode].GetCenter();
	}

	FVector2D CurrentSpellNodePosition = SpellNodes[(int32)CurrentSpellNodes[0]].GetCenter();
	float OptimalDistance = FVector2D::Distance(LastSpellNodePosition, CurrentSpellNodePosition);
	float Quality = -FMath::Abs(OptimalDistance - CurrentDistance) / OptimalDistance;

	CurrentDistance = 0.0f;

	FSpellComponent SpellComponent;
	SpellComponent.SpellNode = CurrentSpellNodes[0];
	SpellComponent.Quality = Quality;

	CurrentSpellComponents.Add(SpellComponent);

	SpellCastNodeReached.Broadcast(CurrentSpellNodes[0], CurrentSpellNodePosition, Quality);
}

void USpellManagementComponent::Reset()
{
	CurrentSpellComponents.Empty();
	LastMousePosition = FVector2D(0.0f, 0.0f);
	CurrentDistance = 0.0f;
}

void USpellManagementComponent::GetSpell(FSpell & Spell, float & Quality, bool & Valid)
{
	uint32 Key = 0;
	uint32 Offset = 0;
	Quality = 0.0f;
	for (FSpellComponent const & SpellComponent : CurrentSpellComponents) {
		Key += (uint32)SpellComponent.SpellNode + Offset;
		Offset += SpellNodes.Num();
		Quality += SpellComponent.Quality;
	}
	
	if (Spells.Contains(Key))
	{
		Spell = *Spells.Find(Key);
		Valid = true;
	}
	else
	{
		Valid = false;
	}
}


void USpellManagementComponent::AddSpell(FSpell const & Spell)
{
	uint32 Key = 0;
	uint32 Offset = 0;
	for (ESpellNode const & SpellNode : Spell.SpellNodes)
	{
		Key += (uint32) SpellNode + Offset;
		Offset += SpellNodes.Num();
	}
	Spells.Add(Key, Spell);
}
