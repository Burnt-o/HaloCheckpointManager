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
#include "SpriteResource.h"
#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"
#include "directxtk\CommonStates.h"
#include "MultilevelPointer.h"
#include "directxtk\PrimitiveBatch.h"
#include "directxtk\GeometricPrimitive.h"
#include "directxtk\Effects.h"
#include <wrl/client.h>

// impl in .cpp
template<GameState::Value mGame>
class Renderer3DImpl : public IRenderer3D
{
private:
	//struct CameraFrustumFaces
	//{
	//	std::array<SimpleMath::Vector3, 4> nearFrustum, farFrustum, leftFrustum, rightFrustum, topFrustum, bottomFrustum;
	//};

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

	// map of loaded resource IDs to their respective spriteResource.
	// If drawSprite is passed an ID not in this map, will call loadSpriteResource to try to load it and add to the map.
	std::map<int, std::unique_ptr<SpriteResource>> spriteResourceCache {}; // starts empty

	std::unique_ptr<GeometricPrimitive> unitCube;
	std::unique_ptr<GeometricPrimitive> unitCubeInverse; // backfaces on the front
	std::unique_ptr<GeometricPrimitive> unitSphere;
	std::shared_ptr<PrimitiveBatch<VertexPosition>> primitiveDrawer;

	std::unique_ptr<BasicEffect> primitiveBatchEffect;
	Microsoft::WRL::ComPtr< ID3D11InputLayout > inputLayout;
	ID3D11ShaderResourceView* patternedTexture;

	void createDepthStencilView(SimpleMath::Vector2 screenSize);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthStencilState;

	void constructSpriteResource(int resourceID); // throws HCMRuntimeExceptions on fail
	void initialise(); // run on first render frame to init spriteBatch & common states

	// injected services
	std::weak_ptr<GetGameCameraData> getGameCameraDataWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// funcs
	virtual bool updateCameraData(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView) override;
	RECTF drawSpriteImpl(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor, bool shouldCenter);
	friend class Render3DEventProvider;






public:
	Renderer3DImpl(GameState game, IDIContainer& dicon);
	~Renderer3DImpl();

	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind) override;
	virtual SimpleMath::Vector3 worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition, int screenEdgeOffset, bool* appliedClamp) override;
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) override;
	virtual RECTF drawSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual RECTF drawCenteredSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual bool pointOnScreen(const SimpleMath::Vector3& worldPointPosition) override;
	virtual bool pointBehindCamera(const SimpleMath::Vector3& worldPointPosition) override;

	virtual void renderTriggerModelSolid(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerInteriorStyle interiorStyle) override;
	virtual void renderSphere(const SimpleMath::Vector3& position, const SimpleMath::Vector4& color, const float& scale, const bool& isWireframe) override;
	virtual std::shared_ptr<PrimitiveBatch<VertexPosition>> getPrimitiveDrawer() override{ return primitiveDrawer; }
	virtual void setPrimitiveColor(const SimpleMath::Vector4& color) 
	{ 
		primitiveBatchEffect->SetColorAndAlpha(color);
		primitiveBatchEffect->Apply(this->pDeviceContext);
	}

	virtual const SimpleMath::Vector3 getCameraPosition() override { return cameraPosition; };

	//virtual void renderTriangle(const std::array<SimpleMath::Vector3, 3>& vertices, const SimpleMath::Vector4& color) override;
	//virtual void renderTriggerModel(const TriggerModel& model, const SimpleMath::Vector4& triggerColor, const SettingsEnums::TriggerRenderStyle renderStyle, const SettingsEnums::TriggerInteriorStyle interiorStyle, const SettingsEnums::TriggerLabelStyle labelStyle, const float labelScale) override;



};