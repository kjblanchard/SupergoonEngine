#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/ui/nineslice.h>
#include <stdio.h>

static void UINinesliceDraw(UIObject* o) {
	UINineSliceData* data = (UINineSliceData*)o->TypeData;
	if (!data) {
		return;
	}
	Color color = {255, 255, 255, 255};
	RectangleF dst = {o->AbsolutePos.X, o->AbsolutePos.Y, o->Rect.w, o->Rect.h};
	DrawTexture(data->Texture, GetDefaultShader(), &dst, &data->SourceRect, false, 1.0f, false, &color);
}

static void UINinesliceDestroy(UIObject* o) {
	UINineSliceData* data = (UINineSliceData*)o->TypeData;
	if (!data) {
		return;
	}
	TextureDestroy(data->Texture);
}

static UIType uininesliceType = {"Image", NULL, UINinesliceDraw, UINinesliceDestroy};

UIObject* CreateUINineSlice(UINineSliceArgs* args) {
	UINineSliceData* data = malloc(sizeof(*data));
	args->Object->Type = &uininesliceType;
	args->Object->TypeData = data;
	Texture* renderTargetTexture = TextureCreateRenderTarget((int)args->Object->Rect.w, (int)args->Object->Rect.h);
	int nineSliceImageW = TextureGetWidth(args->Texture);
	int nineSliceImageH = TextureGetHeight(args->Texture);
	data->SourceRect = (RectangleF){0, 0, args->Object->Rect.w, args->Object->Rect.h};
	// This counts as the middle
	TextureClearRenderTarget(renderTargetTexture, args->Color.R / (float)255, args->Color.G / (float)255, args->Color.B / (float)255, args->Color.A / (float)255);
	float sizeX = args->NineSliceOffset.X;
	float sizeY = args->NineSliceOffset.Y;
	// // // / Draw the corners
	// // // tl
	RectangleF srcRect = {0, 0, sizeX, sizeY};
	RectangleF dstRect = {0, 0, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// tr
	srcRect = (RectangleF){nineSliceImageW - sizeX, 0, sizeX, sizeY};
	dstRect = (RectangleF){args->Object->Rect.w - sizeX, 0, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// // // bl
	srcRect = (RectangleF){0, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){0, args->Object->Rect.h - sizeY, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// // br
	srcRect = (RectangleF){nineSliceImageW - sizeX, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){args->Object->Rect.w - sizeX, args->Object->Rect.h - sizeY, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// // draw the bars
	int length = args->Object->Rect.w - (sizeX);
	int height = args->Object->Rect.h - (sizeY);
	// // top
	srcRect = (RectangleF){1 + sizeX, 0, 1, sizeY};
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, 0, 1, sizeY};
		DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// // bottom
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, args->Object->Rect.h - sizeY + 4, 1, sizeY};
		DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// // left
	srcRect = (RectangleF){0, sizeY + 1, sizeX, 1};
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){0, (float)i, sizeX, 1};
		DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// // right
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){args->Object->Rect.w - sizeX + 3, (float)i, sizeX, 1};
		DrawTextureToTexture(renderTargetTexture, args->Texture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	data->Texture = renderTargetTexture;
	return args->Object;
}
