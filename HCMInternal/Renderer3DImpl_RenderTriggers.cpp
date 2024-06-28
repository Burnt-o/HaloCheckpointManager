#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "RenderTextHelper.h"
#include "directxtk\GeometricPrimitive.h"
#include "directxtk\CommonStates.h"
#include "directxtk\Effects.h"

using namespace SettingsEnums;






template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModelSolid(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerInteriorStyle interiorStyle)
{
	LOG_ONCE(PLOG_DEBUG << "renderTriggerModelSolid");
	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;

	bool cameraInsideTrigger = model.box.Contains(this->cameraPosition);

	if (cameraInsideTrigger && interiorStyle == TriggerInteriorStyle::DontRender)
		return;

	// draw solid volume
	if (cameraInsideTrigger)
	{
		assert(interiorStyle != TriggerInteriorStyle::Patterned && "TODO, need to create the patternedTexture Resource");
		ID3D11ShaderResourceView* texture = (cameraInsideTrigger && interiorStyle == TriggerInteriorStyle::Patterned) ? patternedTexture : nullptr;
		unitCubeInverse->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, texture);
	}
	else
	{
		unitCube->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, nullptr);
	}
}

/*
template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerRenderStyle renderStyle, const SettingsEnums::TriggerInteriorStyle interiorStyle, const SettingsEnums::TriggerLabelStyle labelStyle, const float labelScale)
{
	LOG_ONCE(PLOG_DEBUG << "renderTriggerModel");
	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;


	bool cameraInsideTrigger = model.box.Contains(this->cameraPosition);


	if (cameraInsideTrigger && interiorStyle == TriggerInteriorStyle::DontRender)
		return;


	// draw solid volume
	if (renderStyle != TriggerRenderStyle::Wireframe)
	{
		if (cameraInsideTrigger)
		{
			assert(interiorStyle != TriggerInteriorStyle::Patterned && "TODO, need to create the patternedTexture Resource");
			ID3D11ShaderResourceView* texture = (cameraInsideTrigger && interiorStyle == TriggerInteriorStyle::Patterned) ? patternedTexture : nullptr;
			unitCubeInverse->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, texture);
		}
		else
		{
			unitCube->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, nullptr);
		}

	}

	// draw wireframe
	if (renderStyle != TriggerRenderStyle::Solid)
	{


		lineDrawer->Begin();
		for (auto& edge : model.edges)
		{
			VertexPositionColor a = VertexPositionColor(edge.first, triggerColor);
			VertexPositionColor b = VertexPositionColor(edge.second, triggerColor);

			lineDrawer->DrawLine(a, b);
		}
		lineDrawer->End();
	}

	// draw label
	if (labelStyle != TriggerLabelStyle::None)
	{

		auto pointOnScreen = labelStyle == TriggerLabelStyle::Center
			? !pointBehindCamera(model.box.Center)
			: !pointBehindCamera(model.box.Center - ((SimpleMath::Vector3)model.box.Extents / 2)); // corner (TODO: store corner in triggermodel)

		if (pointOnScreen)
		{
			auto screenPosition = labelStyle == TriggerLabelStyle::Center
				? worldPointToScreenPosition(model.box.Center, false)
				: worldPointToScreenPosition(model.box.Center - ((SimpleMath::Vector3)model.box.Extents / 2), false); // corner (TODO: store corner in triggermodel)

			auto dynLabelScale = labelScale * RenderTextHelper::scaleTextDistance(cameraDistanceToWorldPoint(model.box.Center));
			// draw main label text

			if (labelStyle == TriggerLabelStyle::Center)
			{
				RenderTextHelper::drawCenteredOutlinedText(
					model.label,
					{ screenPosition.x, screenPosition.y },
					ImGui::ColorConvertFloat4ToU32(triggerColor) | 0xFFC0C0C0, // remove alpha, brigten a lil bit
					dynLabelScale
				);
			}
			else
			{
				RenderTextHelper::drawOutlinedText(
					model.label,
					{ screenPosition.x, screenPosition.y },
					ImGui::ColorConvertFloat4ToU32(triggerColor) | 0xFFC0C0C0, // remove alpha, brigten a lil bit
					dynLabelScale
				);
			}



		}

	}



}

*/



// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;