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
		ID3D11ShaderResourceView* texture = interiorStyle == TriggerInteriorStyle::Patterned ? patternedTexture : nullptr;
		unitCubeInverse->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, texture);
	}
	else
	{
		unitCube->Draw(model.transformation, this->viewMatrix, this->projectionMatrix, triggerColor, nullptr);
	}
}



// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;