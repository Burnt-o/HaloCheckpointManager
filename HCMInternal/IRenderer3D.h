#pragma once
#include "pch.h"
#include "SpriteResource.h"
#include "TriggerModel.h"
#include "SettingsEnums.h"
#include "directxtk\VertexTypes.h"
#include "directxtk\PrimitiveBatch.h"

/// <summary>
/// Provides functions for rendering in 3D. Implemented by Renderer3DImpl.h
/// <see cref="Renderer3DImpl.h"/>
/// </summary>
class IRenderer3D {
private:
	/// <summary>
	/// Called once per frame to update the internal camera state.
	/// </summary>
	/// <returns>False if an exception occurs.</returns>
	virtual bool updateCameraData(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView) = 0;
	friend class Render3DEventProvider;
public:

	/// <summary>
	/// Calculates the screen position of a world position.
	/// </summary>
	/// <param name="worldPointPosition">The 3D world position.</param>
	/// <param name="shouldFlipBehind">Whether points behind the camera should be flipped to be on the correct side.</param>
	/// <returns>Vector3 with screen space position in pixels in x (width, 0 is left), y (height, 0 is top), and z is clipspace depth.</returns>
	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind = true) = 0;

	/// <summary>
	/// Calculates the screen position of a world position, clamping points outside the view frustrum to the edge of the screen.
	/// </summary>
	/// <param name="worldPointPosition">The 3D world position.</param>
	/// <param name="screenEdgeOffset">How far in pixels from the screen to offset clamped positions (negative numbers outside the screen, 0 on edge, positive into the screen)</param>
	/// <param name="appliedClamp">Out - whether the initial world point was in the view frustrum (ie was it clamped?)</param>
	/// <returns>Vector3 with screen space position in pixels in x (width, 0 is left), y (height, 0 is top), and z is clipspace depth.</returns>
	virtual SimpleMath::Vector3 worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition, int screenEdgeOffset = 0, bool* appliedClamp = nullptr) = 0;

	/// <summary>
	/// Calculates how far from the camera a world position is.
	/// </summary>
	/// <param name="worldPointPosition">The 3D world position.</param>
	/// <returns>The distance between the camera and world position.</returns>
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) = 0;

	/// <summary>
	/// Checks if a world position will be visible on screen (is inside the view frustrum)
	/// </summary>
	/// <param name="worldPointPosition">The 3D world position.</param>
	/// <returns>True if world position will be visible.</returns>
	virtual bool pointOnScreen(const SimpleMath::Vector3& worldPointPosition) = 0;



	/// <summary>
	/// Checks if a world position will in the 180 degree semisphere behind the camera
	/// </summary>
	/// <param name="worldPointPosition">The 3D world position.</param>
	/// <returns>True if world position will be behind the camera.</returns>
	virtual bool pointBehindCamera(const SimpleMath::Vector3& worldPointPosition) = 0;

	/// <summary>
	/// Draws a 2D sprite at the specified screen position (anchored top-left). 
	/// </summary>
	/// <param name="spriteResourceID">Identifier of the sprite to draw (see HCMInternal.rc).</param>
	/// <param name="screenPosition">Screen position to draw the sprite (this will be the top-left corner of the sprite).</param>
	/// <param name="spriteScale">Scaling factor of sprite.</param>
	/// <param name="spriteColor">What colour should be overlaid on the sprite.</param>
	/// <returns></returns>
	virtual RECTF drawSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale = 1.f, SimpleMath::Vector4 spriteColor = {1.f, 0.5f, 0.f, 1.f}) = 0;

	/// <summary>
	/// Draws a 2D sprite at the specified screen position (center of sprite). 
	/// </summary>
	/// <param name="spriteResourceID">Identifier of the sprite to draw (see HCMInternal.rc).</param>
	/// <param name="screenPosition">Screen position to draw the sprite (this will be the center of the sprite).</param>
	/// <param name="spriteScale">Scaling factor of sprite.</param>
	/// <param name="spriteColor">What colour should be overlaid on the sprite.</param>
	/// <returns></returns>
	virtual RECTF drawCenteredSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale = 1.f, SimpleMath::Vector4 spriteColor = { 1.f, 0.5f, 0.f, 1.f }) = 0;

	/// <summary>
	/// Renders a trigger model to the screen with given colours as a solid volume.
	/// </summary>
	/// <param name="model">The trigger model to draw.</param>
	/// <param name="triggerColor">What colour to draw it (alpha supported).</param>
	/// <param name="interiorStyle">When camera inside trigger, draw normally, with a patterned texture, or not at all.</param>
	virtual void renderTriggerModelSolid(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerInteriorStyle interiorStyle) = 0;

	/// <summary>
	/// Gets the primitiveBatch drawer.
	/// </summary>
	/// <returns>Shared ptr to the primitive batch drawer.</returns>
	virtual std::shared_ptr<PrimitiveBatch<VertexPosition>> getPrimitiveDrawer() = 0;

	/// <summary>
	/// Sets the color of objects drawn by primitiveBatch drawer.
	/// </summary>
	/// <param name="color">Float4 of the colour to be drawn.</param>
	virtual void setPrimitiveColor(const SimpleMath::Vector4& color) = 0;

	/// <summary>
	/// Draws a unit sphere (scaled by param) of the specified colour at the specified position.
	/// </summary>
	/// <param name="position">Float3 world position of the center of the sphere.</param>
	/// <param name="color">Float4 of the colour to be drawn.</param>
	/// <param name="scale">Float of scaling (unit scale) to apply to the sphere.</param>
	/// <param name="isWireframe">Whether should be drawn as wireframe. Otherwise, filled volume. </param>
	virtual void renderSphere(const SimpleMath::Vector3& position, const SimpleMath::Vector4& color, const float& scale, const bool& isWireframe) = 0;

	/// <summary>
	/// Gets the cameras current position.
	/// </summary>
	/// <returns>worldPoint position of camera</returns>
	virtual const SimpleMath::Vector3 getCameraPosition() = 0;


};
