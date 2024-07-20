#pragma once
#include "pch.h"
#include "DIContainer.h"
#include "IRenderer3D.h"
#include "GameState.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "IMCCStateHook.h"
#include "GetGameCameraData.h"
#include "IMakeOrGetCheat.h"
#include "SettingsStateAndEvents.h"
#include <d3d11.h>
#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"
#include "directxtk\CommonStates.h"
#include "MultilevelPointer.h"
#include "directxtk\PrimitiveBatch.h"
#include "directxtk\GeometricPrimitive.h"
#include "directxtk\Effects.h"
#include <wrl/client.h>
#include "TextureResource.h"

using namespace Microsoft::WRL;

// impl in .cpp
template<GameState::Value mGame>
class Renderer3DImpl : public IRenderer3D
{
private:
	float renderDistance;
	ScopedCallback<eventpp::CallbackList<void(float&)>> renderDistanceChangedCallback;

	struct CameraFrustumSidePlanes
	{
		SimpleMath::Plane leftFrustum, rightFrustum, topFrustum, bottomFrustum;
	};

	struct CameraFrustumSideTris
	{
		std::array<SimpleMath::Vector3, 3> leftFrustum1, leftFrustum2, rightFrustum1, rightFrustum2, topFrustum1, topFrustum2, bottomFrustum1, bottomFrustum2;
	};

	// data
	bool haveShownError = false; // prevents updateGameCamera from spamming messages. resets to false on gamestate change.
	SimpleMath::Vector3 cameraPosition;
	SimpleMath::Vector3 cameraDirection;
	SimpleMath::Vector3 cameraUp;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;
	DirectX::SimpleMath::Matrix viewProjectionMatrix;
	SimpleMath::Vector2 lastScreenSize {0, 0};
	SimpleMath::Vector2 screenSize;
	SimpleMath::Vector2 screenCenter;
	DirectX::BoundingFrustum frustumViewWorld;


	// These resources do not belong to us, they belong to the game - do not release them
	ID3D11Device* pDevice; 
	ID3D11DeviceContext* pDeviceContext; 
	ID3D11RenderTargetView* pMainRenderTargetView;

	bool init = false; // initialise spriteBatch & commonStates on first render frame
	std::unique_ptr<SpriteBatch> spriteBatch; // gets remade by constructSpriteResource to update sprite atlas
	std::unique_ptr<CommonStates> commonStates;

	std::optional<std::shared_ptr<MultilevelPointer>> verticalFOVPointer;
	float* pVerticalFOVCached;

	// just for caching verticalFOV, if we have it
	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> mGameStateChangedCallback;
	void onGameStateChanged(const MCCState& newMCCState);

	// map of loaded resource IDs to their respective resourve view.
	std::map<TextureEnum, std::unique_ptr<TextureResource>> textureResources {}; // loaded up in init

	std::unique_ptr<GeometricPrimitive> unitSphere; // used in renderSphere (trigger hit position)
	std::shared_ptr<PrimitiveBatch<VertexPosition>> primitiveDrawer;

	std::unique_ptr<BasicEffect> primitiveBatchEffect;
	ComPtr< ID3D11InputLayout > inputLayoutTextured;
	ComPtr< ID3D11InputLayout > inputLayoutUntextured;

	void createDepthStencilView(SimpleMath::Vector2 screenSize);
	ComPtr<ID3D11Texture2D> depthStencil;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;


	void initialise(); // run on first render frame to init spriteBatch & common states

	// injected services
	std::weak_ptr<GetGameCameraData> getGameCameraDataWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// funcs
	virtual bool updateCameraData(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView) override;
	RECTF drawSpriteImpl(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor, bool shouldCenter);
	friend class Render3DEventProvider;


	void setTexture(std::optional<TextureEnum> texture);
	void setCullMode(CullingOption cullingOption);
public:
	Renderer3DImpl(GameState game, IDIContainer& dicon);
	~Renderer3DImpl();

	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind) override;
	virtual SimpleMath::Vector3 worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition, int screenEdgeOffset, bool* appliedClamp) override;
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) override;
	virtual RECTF drawSprite(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual RECTF drawCenteredSprite(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual bool pointOnScreen(const SimpleMath::Vector3& worldPointPosition) override;
	virtual bool pointBehindCamera(const SimpleMath::Vector3& worldPointPosition) override;

	virtual void renderSphere(const SimpleMath::Vector3& position, const SimpleMath::Vector4& color, const float& scale, const bool& isWireframe) override;

	virtual const SimpleMath::Vector3 getCameraPosition() override { return cameraPosition; };
	virtual const DirectX::BoundingFrustum& getCameraFrustum() override { return frustumViewWorld; }

	virtual void drawTriangle(const std::array<SimpleMath::Vector3, 3>& vertexPositions, const SimpleMath::Vector4& color, CullingOption cullingOption, std::optional<TextureEnum> texture) override;
	virtual void drawTriangleCollection(const IModelTriangles* model, const SimpleMath::Vector4& color, CullingOption cullingOption, std::optional<TextureEnum> texture) override;

	virtual void drawEdge(const SimpleMath::Vector3& edgeStart, const SimpleMath::Vector3& edgeEnd, const SimpleMath::Vector4& color) override;
	virtual void drawEdgeCollection(const IModelEdges* model, const SimpleMath::Vector4& color) override;

	//virtual void renderTriangle(const std::array<SimpleMath::Vector3, 3>& vertices, const SimpleMath::Vector4& color) override;
	//virtual void renderTriggerModel(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerRenderStyle renderStyle, const SettingsEnums::TriggerInteriorStyle interiorStyle, const SettingsEnums::TriggerLabelStyle labelStyle, const float labelScale) override;



};