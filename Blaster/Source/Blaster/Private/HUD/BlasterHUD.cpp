// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewPortSizes;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSizes);
		const FVector2d ViewportCenter(ViewPortSizes.X/2.f,ViewPortSizes.Y/2.f);

		if(HUDPackage.CrossHairCenter)
		{
			DrawCrossHair(HUDPackage.CrossHairCenter,ViewportCenter);
		}
		if(HUDPackage.CrossHairDown)
		{
			DrawCrossHair(HUDPackage.CrossHairDown,ViewportCenter);
		}
		if(HUDPackage.CrossHairLeft)
		{
			DrawCrossHair(HUDPackage.CrossHairLeft,ViewportCenter);
		}
		if(HUDPackage.CrossHairRight)
		{
			DrawCrossHair(HUDPackage.CrossHairRight,ViewportCenter);
		}
		if(HUDPackage.CrossHairUp)
		{
			DrawCrossHair(HUDPackage.CrossHairUp,ViewportCenter);
		}
	}
}

void ABlasterHUD::DrawCrossHair(UTexture2D* Texture, FVector2d ViewPOrtCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2d TextureDrawPoint(
	ViewPOrtCenter.X - (TextureWidth/2.f),ViewPOrtCenter.Y -(TextureHeight/2.f)
	);
	DrawTexture(
	Texture,
	TextureDrawPoint.X,
	TextureDrawPoint.Y,
	TextureWidth,
	TextureHeight,
	0.f,0.f,
	1.f,1.f,
	FLinearColor::White
	);
}
