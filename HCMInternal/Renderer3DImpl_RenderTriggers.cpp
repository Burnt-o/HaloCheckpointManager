#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "RenderTextHelper.h"
#include "directxtk\GeometricPrimitive.h"
#include "directxtk\CommonStates.h"
#include "directxtk\Effects.h"



template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor)
{
	LOG_ONCE(PLOG_DEBUG << "renderTriggerModel");
	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;






#ifndef HCM_DEBUG
	static_assert(false && "todo: cache this in triggerData/model");
#endif
	// right.. geometric primitive can't handle oriented boxes (ie rotated triggers). we'll ignore orientation for now but we'll need to switch to CreateCustom w/ vertex collection etc
	auto triggerShape = DirectX::GeometricPrimitive::CreateBox(pDeviceContext, model.box.Extents);

	auto localTransform = worldTransformation * SimpleMath::Matrix::CreateTranslation(model.box.Center);
	triggerShape->Draw(localTransform, viewMatrix, projectionMatrix, {0.f, 1.f, 0.f, 0.5f});

	// the only problem with this wireframe is that it's wireframing the tris, not the quads lol.
	// could achieve with shader effect maybe? 
	// or textures.. but matching the texture scaling to face size could be painful.
	triggerShape->Draw(localTransform, viewMatrix, projectionMatrix, { 0.f, 1.f, 0.f, 1.f }, nullptr, true);


	// also boxes stop rendering if you're inside them (backface culling) - can prevent this by doing a duplicate drawing with invert and rhand coord args flipped.

#ifndef HCM_DEBUG
	static_assert(false && "colour args need to go back to being vec4");
#endif


	// TODO: effects and textures and wireframe and stuff https://github.com/microsoft/DirectXTK/wiki/GeometricPrimitive





}




// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;