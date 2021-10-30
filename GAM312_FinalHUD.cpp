// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312_FinalHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AGAM312_FinalHUD::AGAM312_FinalHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	/* Set Widget Health Bar */
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Health_UI"));
	HUDWidgetClass = HealthBarObj.Class;
}


void AGAM312_FinalHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair
	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void AGAM312_FinalHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr) // If HUDWidgetClass does exist.
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass); // Set the current widget.

		if (CurrentWidget) // And if it does exist.
		{
			CurrentWidget -> AddToViewport(); // Add it to screen.
		}
	}
}