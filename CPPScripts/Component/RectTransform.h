#pragma once
#include "Transform.h"

namespace ZXEngine
{
	enum class UIAnchorVertical
	{
		Top,
		Middle,
		Bottom
	};

	enum class UIAnchorHorizontal
	{
		Left,
		Center,
		Right
	};

	class RectTransform : public Transform
	{
	public:
		static ComponentType GetType();

	public:
		UIAnchorVertical mAnchorV = UIAnchorVertical::Middle;
		UIAnchorHorizontal mAnchorH = UIAnchorHorizontal::Center;

		RectTransform() = default;
		~RectTransform() = default;

		virtual ComponentType GetInsType();
	};
}