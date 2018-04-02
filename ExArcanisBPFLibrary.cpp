// Fill out your copyright notice in the Description page of Project Settings.

#include "ExArcanisBPFLibrary.h"
#include "ExArcanis.h"
#include "Engine.h"

//// DYNAMIC TEXTURE ////

void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData) {
	if (Texture->Resource) {
		struct FUpdateTextureRegionsData {
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex) {
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip) {
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
							);
					}
				}
		if (bFreeData) {
			FMemory::Free(RegionData->Regions);
			//FMemory::Free(RegionData->SrcData);
		}
		delete RegionData;
			});
	}
}

void UExArcanisBPFLibrary::UpdateTextureFromColorData(const TArray<FLinearColor>& ColorData, UTexture2D* Texture) {
	// Sanity checks.
	if (!Texture)
		return;

	if (ColorData.Num() <= 0)
		return;



	// Update region.
	FUpdateTextureRegion2D *region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSurfaceWidth(), Texture->GetSurfaceHeight());

	// Call the render command.
	//UpdateTextureRegions(Texture, 0, 1, region, Texture->GetSurfaceWidth() * 16, 16, (uint8*)ColorData.GetTypedData(), true); PROBLEM IN 4.18
}

//// MOUSE CURSOR ////

void UExArcanisBPFLibrary::SetMousePosition(const APlayerController* ThePC, const float& PosX, const float& PosY) {
	if (!ThePC) 
		return;
	//~~~~~~~~~~~~~

	//Get Player
	const ULocalPlayer * MyPlayer = Cast<ULocalPlayer>(ThePC->Player);
	//PlayerController::Player is UPlayer

	if (!MyPlayer) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	//get view port ptr
	const UGameViewportClient * ViewportClient =
		Cast < UGameViewportClient >(MyPlayer->ViewportClient);

	if (!ViewportClient) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	FViewport * Viewport = ViewportClient->Viewport;

	if (!Viewport) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	//Set Mouse
	Viewport->SetMouse(int32(PosX), int32(PosY));
}

void UExArcanisBPFLibrary::GetMousePosition(const APlayerController * ThePC, float & PosX, float & PosY) {
	if (!ThePC) 
		return;
	//~~~~~~~~~~~~~

	//Get Player
	const ULocalPlayer * MyPlayer = Cast<ULocalPlayer>(ThePC->Player);
	//PlayerController::Player is UPlayer

	if (!MyPlayer) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	//get view port ptr
	const UGameViewportClient * ViewportClient =
		Cast < UGameViewportClient >(MyPlayer->ViewportClient);

	if (!ViewportClient) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	FViewport * Viewport = ViewportClient->Viewport;

	if (!Viewport) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	PosX = float(Viewport->GetMouseX());
	PosY = float(Viewport->GetMouseY());
}

void UExArcanisBPFLibrary::GetCenterOfViewport(const APlayerController * ThePC, float & PosX, float & PosY) {
	if (!ThePC) 
		return;
	//~~~~~~~~~~~~~

	//Get Player
	const ULocalPlayer * MyPlayer = Cast<ULocalPlayer>(ThePC->Player);
	//PlayerController::Player is UPlayer

	if (!MyPlayer)
		return;
	//~~~~~~~~~~~~~~~~~~~~

	//get view port ptr
	const UGameViewportClient * ViewportClient =
		Cast < UGameViewportClient >(MyPlayer->ViewportClient);

	if (!ViewportClient) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	FViewport * Viewport = ViewportClient->Viewport;

	if (!Viewport) 
		return;
	//~~~~~~~~~~~~~~~~~~~~

	//Get Size
	FIntPoint Size = Viewport->GetSizeXY();

	//Center
	PosX = Size.X / 2;
	PosY = Size.Y / 2;
}

bool UExArcanisBPFLibrary::IsGamepadConnected() {
	auto genericApplication = FSlateApplication::Get().GetPlatformApplication();
	if (genericApplication.Get() != nullptr && genericApplication->IsGamepadAttached()) {
		return true;
	}
	return false;
}