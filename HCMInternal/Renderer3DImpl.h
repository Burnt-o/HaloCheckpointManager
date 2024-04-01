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
	SimpleMath::Vector2 screenSize;
	SimpleMath::Vector2 screenCenter;
	DirectX::BoundingFrustum frustumViewWorld;
	DirectX::BoundingFrustum frustumViewWorldBackwards;
	std::array<std::array<SimpleMath::Vector3, 4>, 6> frustumViewWorldFaces;
	CameraFrustumSidePlanes frustumViewWorldSidePlanes;
	std::array<SimpleMath::Plane, 6> frustumViewWorldPlanes;
	CameraFrustumSideTris frustumViewWorldSideTris;
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


	bool pointBehindCamera(const SimpleMath::Vector3& point);
	std::vector<SimpleMath::Vector3> clipFaceToFrustum(const faceView& face);
	void renderFace(const faceView& face, uint32_t color, bool debugVertices = false);

	std::optional<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> clipLineSegmentToFrustum(const SimpleMath::Vector3& start, const SimpleMath::Vector3& end);
	void renderEdge(const edgeView& edge, uint32_t color);


public:
	Renderer3DImpl(GameState game, IDIContainer& dicon);
	~Renderer3DImpl();

	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind) override;
	virtual SimpleMath::Vector3 worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition, int screenEdgeOffset, bool* appliedClamp) override;
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) override;
	virtual RECTF drawSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual RECTF drawCenteredSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor) override;
	virtual bool pointOnScreen(const SimpleMath::Vector3& worldPointPosition) override;
	virtual void renderTriggerModel(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor) override;

#ifdef HCM_DEBUG
	virtual void renderTriggerModelSortedDebug(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor) override;
#endif

};