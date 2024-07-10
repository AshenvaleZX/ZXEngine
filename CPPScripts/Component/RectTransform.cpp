#include "RectTransform.h"
#include "../GameObject.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	ComponentType RectTransform::GetType()
	{
		return ComponentType::RectTransform;
	}

	ComponentType RectTransform::GetInsType()
	{
		return ComponentType::RectTransform;
	}

	void RectTransform::SetWidth(float width)
	{
		mWidth = width;

		UpdateLocalRectPosition();
		UpdateChildrenLocalPosition();
	}

	float RectTransform::GetWidth() const
	{
		return mWidth;
	}

	void RectTransform::SetHeight(float height)
	{
		mHeight = height;

		UpdateLocalRectPosition();
		UpdateChildrenLocalPosition();
	}

	float RectTransform::GetHeight() const
	{
		return mHeight;
	}

	void RectTransform::SetSize(float width, float height)
	{
		mWidth = width;
		mHeight = height;

		UpdateLocalRectPosition();
		UpdateChildrenLocalPosition();
	}

	void RectTransform::SetVerticalAnchor(UIAnchorVertical anchorV)
	{
		mAnchorV = anchorV;

		UpdateLocalRectPosition();
	}

	void RectTransform::SetHorizontalAnchor(UIAnchorHorizontal anchorH)
	{
		mAnchorH = anchorH;

		UpdateLocalRectPosition();
	}

	void RectTransform::SetLocalRectPosition(const Vector2& position)
	{
		mLocalRectPosition.x = position.x;
		mLocalRectPosition.y = position.y;

		UpdateLocalPosition();
	}

	void RectTransform::SetLocalRectPosition(const Vector3& position)
	{
		mLocalRectPosition = position;

		UpdateLocalPosition();
	}

	void RectTransform::SetLocalRectPosition(float x, float y, float z)
	{
		mLocalRectPosition.x = x;
		mLocalRectPosition.y = y;
		mLocalRectPosition.z = z;

		UpdateLocalPosition();
	}

	Vector3 RectTransform::GetLocalRectPosition() const
	{
		return mLocalRectPosition;
	}

	void RectTransform::UpdateLocalPosition()
	{
		float width = 0.0f;
		float height = 0.0f;
		auto parent = gameObject->parent;
		RectTransform* parentRectTransform = nullptr;
		if (parent)
			parentRectTransform = parent->GetComponent<RectTransform>();

		if (parentRectTransform)
		{
			width = parentRectTransform->mWidth;
			height = parentRectTransform->mHeight;
		}
		else
		{
			width = static_cast<float>(GlobalData::srcWidth);
			height = static_cast<float>(GlobalData::srcHeight);
		}

		switch (mAnchorV)
		{
		case UIAnchorVertical::Top:
			localPosition.y = mLocalRectPosition.y + height * 0.5f;
			break;
		case UIAnchorVertical::Middle:
			localPosition.y = mLocalRectPosition.y;
			break;
		case UIAnchorVertical::Bottom:
			localPosition.y = mLocalRectPosition.y - height * 0.5f;
			break;
		}

		switch (mAnchorH)
		{
		case UIAnchorHorizontal::Left:
			localPosition.x = mLocalRectPosition.x - width * 0.5f;
			break;
		case UIAnchorHorizontal::Center:
			localPosition.x = mLocalRectPosition.x;
			break;
		case UIAnchorHorizontal::Right:
			localPosition.x = mLocalRectPosition.x + width * 0.5f;
			break;
		}

		localPosition.z = mLocalRectPosition.z;
	}

	void RectTransform::UpdateLocalRectPosition()
	{
		float width = 0.0f;
		float height = 0.0f;
		auto parent = gameObject->parent;
		RectTransform* parentRectTransform = nullptr;
		if (parent)
			parentRectTransform = parent->GetComponent<RectTransform>();

		if (parentRectTransform)
		{
			width = parentRectTransform->mWidth;
			height = parentRectTransform->mHeight;
		}
		else
		{
			width = static_cast<float>(GlobalData::srcWidth);
			height = static_cast<float>(GlobalData::srcHeight);
		}

		switch (mAnchorV)
		{
		case UIAnchorVertical::Top:
			mLocalRectPosition.y = localPosition.y - height * 0.5f;
			break;
		case UIAnchorVertical::Middle:
			mLocalRectPosition.y = localPosition.y;
			break;
		case UIAnchorVertical::Bottom:
			mLocalRectPosition.y = localPosition.y + height * 0.5f;
			break;
		}

		switch (mAnchorH)
		{
		case UIAnchorHorizontal::Left:
			mLocalRectPosition.x = localPosition.x + width * 0.5f;
			break;
		case UIAnchorHorizontal::Center:
			mLocalRectPosition.x = localPosition.x;
			break;
		case UIAnchorHorizontal::Right:
			mLocalRectPosition.x = localPosition.x - width * 0.5f;
			break;
		}

		mLocalRectPosition.z = localPosition.z;
	}

	void RectTransform::UpdateChildrenLocalPosition()
	{
		for (auto child : gameObject->children)
		{
			auto rectTransform = child->GetComponent<RectTransform>();
			if (rectTransform)
				rectTransform->UpdateLocalPosition();
		}
	}

	void RectTransform::UpdateChildrenLocalRectPosition()
	{
		for (auto child : gameObject->children)
		{
			auto rectTransform = child->GetComponent<RectTransform>();
			if (rectTransform)
				rectTransform->UpdateLocalRectPosition();
		}
	}
}