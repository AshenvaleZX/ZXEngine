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

		void SetWidth(float width);
		float GetWidth() const;
		void SetHeight(float height);
		float GetHeight() const;
		void SetSize(float width, float height);
		void SetVerticalAnchor(UIAnchorVertical anchorV);
		void SetHorizontalAnchor(UIAnchorHorizontal anchorH);

		void SetLocalRectPosition(const Vector2& position);
		void SetLocalRectPosition(const Vector3& position);
		void SetLocalRectPosition(float x, float y, float z = 0.0f);
		Vector3 GetLocalRectPosition() const;

	private:
		float mWidth = 100.0f;
		float mHeight = 100.0f;
		Vector3 mLocalRectPosition;

		void UpdateLocalPosition();
		void UpdateLocalRectPosition();
		void UpdateChildrenLocalPosition();
		void UpdateChildrenLocalRectPosition();
	};
}