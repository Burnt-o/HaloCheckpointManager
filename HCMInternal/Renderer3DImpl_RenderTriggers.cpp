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

template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderSphere(const SimpleMath::Vector3& position, const SimpleMath::Vector4& color, const float& scale, const bool& isWireframe)
{
	// create position and scale transforms
	auto resizeTransform = SimpleMath::Matrix::CreateScale(scale);
	auto translateTransform = SimpleMath::Matrix::CreateTranslation(position);

	unitSphere->Draw(resizeTransform * translateTransform, this->viewMatrix, this->projectionMatrix, color, nullptr, isWireframe);
}

template<GameState::Value mGame>
void Renderer3DImpl<mGame>::drawTriangle(const std::array<SimpleMath::Vector3, 3>& vertexPositions, const SimpleMath::Vector4& color)
{
	this->primitiveBatchEffect->SetTextureEnabled(false);
	//this->pDeviceContext->IASetInputLayout(inputLayout);

	this->pDeviceContext->OMSetBlendState(this->commonStates->AlphaBlend(), nullptr, 0xFFFFFFFF); 
	this->pDeviceContext->OMSetDepthStencilState(this->commonStates->DepthRead(), 0); // or depth default?
	this->pDeviceContext->RSSetState(this->commonStates->CullNone());

	this->primitiveBatchEffect->SetColorAndAlpha(color);
	this->primitiveBatchEffect->Apply(pDeviceContext);

	primitiveDrawer->Begin();
	primitiveDrawer->DrawTriangle(vertexPositions[0], vertexPositions[1], vertexPositions[2]);
	primitiveDrawer->End();
}


template<GameState::Value mGame>
void Renderer3DImpl<mGame>::drawEdge(const SimpleMath::Vector3& edgeStart, const SimpleMath::Vector3& edgeEnd, const SimpleMath::Vector4& color)
{
	this->primitiveBatchEffect->SetTextureEnabled(false);
	//this->pDeviceContext->IASetInputLayout(inputLayout);

	this->pDeviceContext->OMSetBlendState(this->commonStates->AlphaBlend(), nullptr, 0xFFFFFFFF);
	this->pDeviceContext->OMSetDepthStencilState(this->commonStates->DepthRead(), 0); // or depth default?
	this->pDeviceContext->RSSetState(this->commonStates->CullNone());

	this->primitiveBatchEffect->SetColorAndAlpha(color);
	this->primitiveBatchEffect->Apply(this->pDeviceContext);

	primitiveDrawer->Begin();
	primitiveDrawer->DrawLine(edgeStart, edgeEnd);
	primitiveDrawer->End();
}
/*
template<GameState::Value mGame>
void Renderer3DImpl<mGame>::drawFilledTris(const VertexCollection& vertices, const IndexCollection& indices, const SimpleMath::Vector4& colour, TextureEnum texture)
{
	
	// https://github.com/microsoft/DirectXTK/blob/main/Src/GeometricPrimitive.cpp#L119
	ID3D11InputLayout* inputLayout;
	if (texture != TextureEnum::NoTexture)
	{
		effect->SetTextureEnabled(true);
		effect->SetTexture(texture);

		inputLayout = mResources->inputLayoutTextured.Get();
	}
	else
	{
		effect->SetTextureEnabled(false);
		inputLayout = mResources->inputLayoutUntextured.Get();
	}


	// if we use a vector of inputLayouts we can select inputLayout using inputLayouts.at((int)textureEnum)

	this->primitiveBatchEffect->SetColorAndAlpha(colour);

	const float alpha = XMVectorGetW(colour);

	if (alpha < 1.f)
	{
		// Alpha blended rendering.
		blendState = stateObjects->AlphaBlend();
		depthStencilState = stateObjects->DepthRead();
	}
	else
	{
		// Opaque rendering.
		blendState = stateObjects->Opaque(); // this enables proper depth occlusion.. 
		depthStencilState = stateObjects->DepthDefault(); // not sure difference between depthDefault and depthRead. former writes?
	}

	this->pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	this->pDeviceContext->OMSetDepthStencilState(depthStencilState, 0);

	this->pDeviceContext->RSSetState(this->commonStates->CullNone()); // or cull counter clockwise?

	// wtf is this? shader stuff? can probably just set once in updateCameraData..
	ID3D11SamplerState* samplerState = this->commonStates->LinearWrap();
	this->pDeviceContext->PSSetSamplers(0, 1, &samplerState);


	// this is why textures were bleeding into barrier earlier.. 
	this->pDeviceContext->IASetInputLayout(inputLayout);
	this->primitiveBatchEffect->Apply(pDeviceContext);



	primitiveDrawer->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices.data(), indices.size(), vertices.data(), vertices.size());

	
}*/

// also need a draw lines with the same sig (minus the texture)

/*

D3D11_PRIMITIVE_TOPOLOGY_LINELIST for wireframe
trianglelist for solid ? 

TriggerData 
.. vertices and indices?
DrawIndexed ? 
a bounding box has 8 vertexes
6 faces = 12 tris
so 12 * 3 indices for the solid volumes

for wireframe:
8 faces = 12 lines
so 12 * 2 indices for the wireframe



barrier overlay: just do each tri one at a time


texturing: steal code from geometric primitive 

  ID3D11InputLayout *inputLayout;
	if (texture)
	{
		effect->SetTextureEnabled(true);
		effect->SetTexture(texture);

		inputLayout = mResources->inputLayoutTextured.Get();
	}
	else
	{
		effect->SetTextureEnabled(false);

		inputLayout = mResources->inputLayoutUntextured.Get();
	}

	input layout gets reset with every draw!
	effect gets applied with every draw!


	 using IndexCollection = std::vector<uint16_t>;
	 using VertexCollection = std::vector<DirectX::VertexPosition>;


*/


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;