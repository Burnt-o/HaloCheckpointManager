#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "RenderTextHelper.h"
#include "directxtk\GeometricPrimitive.h"
#include "directxtk\CommonStates.h"
#include "directxtk\Effects.h"

using namespace SettingsEnums;

template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(const TriggerModel& model, SimpleMath::Vector4& triggerColor, const TriggerRenderStyle renderStyle, const TriggerInteriorStyle interiorStyle, std::optional<float> labelScale)
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
		assert(interiorStyle != TriggerInteriorStyle::Patterned && "TODO, need to create the patternedTexture Resource");
		ID3D11ShaderResourceView* texture = (cameraInsideTrigger && interiorStyle == TriggerInteriorStyle::Patterned) ? patternedTexture : nullptr;
		unitCube->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, texture);
	}

	// set alpha to max for wireframe/label
	triggerColor.w = 1.f;

	// draw wireframe
	if (renderStyle != TriggerRenderStyle::Solid)
	{
		lineDrawer->Begin();
		for (auto& edge : model.edges)
		{
			lineDrawer->DrawLine(edge.first, edge.second);
		}
		lineDrawer->End();
	}

	// draw label
	if (labelScale.has_value())
	{
		auto screenPosition = worldPointToScreenPosition(model.box.Center, false);
		auto dynLabelScale = labelScale.value() * RenderTextHelper::scaleTextDistance(cameraDistanceToWorldPoint(model.box.Center));
		// draw main label text
		RenderTextHelper::drawCenteredOutlinedText(
			model.label,
			{ screenPosition.x, screenPosition.y },
			ImGui::ColorConvertFloat4ToU32(triggerColor),
			dynLabelScale);
	}



}




// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;