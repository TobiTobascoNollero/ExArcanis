// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExArcanisBPFLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EXARCANIS_API UExArcanisBPFLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
		/** Updates a UTexture2D* from an FColor array.
		* Update texture from color data.
		* @param ColorData		Array of FColors to update this texture with. Array is assumed to be the same dimensions as texture, otherwise this call will fail.
		* @param Texture		Texture to update.
		*/
		UFUNCTION(BlueprintCallable, Category = "ExArcanis")
		static void UpdateTextureFromColorData(const TArray<FLinearColor>& ColorData, UTexture2D* Texture);

		/** SET the Mouse Position! Returns false if the operation could not occur */
		UFUNCTION(BlueprintCallable, Category = "ExArcanis")
		static void SetMousePosition(const APlayerController* PlayerController, const float& PosX, const float& PosY);

		/** Get the Cursor Position within the Player's Viewport. This will return a result consistent with SET Mouse Position Returns false if the operation could not occur */
		UFUNCTION(BlueprintPure, Category = "ExArcanis")
		static void GetMousePosition(const APlayerController* PlayerController, float& PosX, float& PosY);

		/** Get the coordinates of the center of the player's screen / viewport. Returns false if the operation could not occur */
		UFUNCTION(BlueprintPure, Category = "ExArcanis")
		static void GetCenterOfViewport(const APlayerController* PlayerController, float& PosX, float& PosY);

		UFUNCTION(BlueprintPure, Category = "ExArcanis")
		static bool IsGamepadConnected();
};
