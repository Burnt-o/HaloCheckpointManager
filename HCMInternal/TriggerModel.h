#pragma once
#include "pch.h"
#include <ranges>

struct VertexInfo { SimpleMath::Vector3 worldPosition, screenPosition; };
typedef std::array< VertexInfo*, 4> faceView;
typedef std::array< VertexInfo*, 2> edgeView;

struct TriggerModel
{
	DirectX::BoundingOrientedBox box; // For frustum culling.
	std::array<VertexInfo, 8> vertices; // Vertices (the real data for rendering).

	void populateViews()
	{
		// Magic numbers from DirectX::g_boxOffset.
		faceViews = {
			faceView { &vertices[0], &vertices[1], &vertices[2], &vertices[3] }, // Near face.
			faceView { &vertices[4], &vertices[5], &vertices[6], &vertices[7] }, // Far face.
			faceView { &vertices[0], &vertices[1], &vertices[5], &vertices[4] },
			faceView { &vertices[0], &vertices[3], &vertices[7], &vertices[4] },
			faceView { &vertices[1], &vertices[2], &vertices[6], &vertices[5] },
			faceView { &vertices[2], &vertices[3], &vertices[7], &vertices[6] },
		};

		edgeViews = {
			// Near face.
			edgeView{&vertices[0], &vertices[1]},
			edgeView{&vertices[1], &vertices[2]},
			edgeView{&vertices[2], &vertices[3]},
			edgeView{&vertices[3], &vertices[0]},

			// Far face.
			edgeView{&vertices[4], &vertices[5]},
			edgeView{&vertices[5], &vertices[6]},
			edgeView{&vertices[6], &vertices[7]},
			edgeView{&vertices[7], &vertices[4]},

			// Connect 'em together.
			edgeView{&vertices[0], &vertices[4]},
			edgeView{&vertices[1], &vertices[5]},
			edgeView{&vertices[2], &vertices[6]},
			edgeView{&vertices[3], &vertices[7]},
		};
	}


	explicit TriggerModel(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
		: box(position, extents, SimpleMath::Quaternion::LookRotation(forward, up))
	{
		DirectX::XMFLOAT3 verticesWorldPosition [8];
		box.GetCorners(verticesWorldPosition);
		for (int i = 0; i < 8; i++)
		{
			vertices[i] = VertexInfo(verticesWorldPosition[i], {0,0,0});
		}

		populateViews();
	}


	// Views into faces. 
	std::array<faceView, 6> faceViews;

	// Views into edges.
	std::array<edgeView, 12> edgeViews;



	// when copying or moving, the "faceViews" and "edgeViews" have their pointers invalidated. Just recalculate them.
	TriggerModel(const TriggerModel& other)
	{
		this->box = other.box;
		this->vertices = other.vertices;
		populateViews();
	}
	TriggerModel(TriggerModel&& other) 
	{
		this->box = other.box;
		this->vertices = other.vertices;
		populateViews();
	}
	TriggerModel& operator=(const TriggerModel& other)
	{
		this->box = other.box;
		this->vertices = other.vertices;
		populateViews();
	}
	TriggerModel& operator=(TriggerModel&& other)
	{
		this->box = other.box;
		this->vertices = other.vertices;
		populateViews();
	}
	
};