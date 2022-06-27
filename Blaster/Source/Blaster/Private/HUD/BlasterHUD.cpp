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

		float SpreadScale = CrossHairPreadMax *HUDPackage.CrossHairSpread;
		if(HUDPackage.CrossHairCenter)
		{
			FVector2d Spread(0.f);
			DrawCrossHair(HUDPackage.CrossHairCenter,ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairDown)
		{
			FVector2d Spread(0.f,SpreadScale);
			DrawCrossHair(HUDPackage.CrossHairDown,ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairLeft)
		{
			FVector2d Spread(-SpreadScale,0.f);
			DrawCrossHair(HUDPackage.CrossHairLeft,ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairRight)
		{
			FVector2d Spread(SpreadScale,0.f);
			DrawCrossHair(HUDPackage.CrossHairRight,ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairUp)
		{
			FVector2d Spread(0.f,-SpreadScale);
			DrawCrossHair(HUDPackage.CrossHairUp,ViewportCenter,Spread);
		}
	}
}

void ABlasterHUD::DrawCrossHair(UTexture2D* Texture, FVector2d ViewPOrtCenter,FVector2d Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2d TextureDrawPoint(
	ViewPOrtCenter.X - (TextureWidth/2.f) + Spread.X
	,ViewPOrtCenter.Y -(TextureHeight/2.f) +Spread.Y
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
