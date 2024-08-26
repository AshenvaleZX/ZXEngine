#include "EditorPanel.h"
#include "EditorGUIManager.h"

namespace ZXEngine
{
    void EditorPanel::CheckManualResize()
	{
        bool isPressing = ImGui::IsMouseDown(0);

        // ����
        if (isPressing && !mPressing)
        {
            ImVec2 currentWindowSize = ImGui::GetWindowSize();
            mSize.x = currentWindowSize.x;
            mSize.y = currentWindowSize.y;
            mDragSize = mSize;
        }
        // �ɿ�
        else if (!isPressing && mPressing)
        {
            if (mSize != mDragSize)
            {
                OnPanelSizeChange(mSize);
            }
        }
        // ��ס
        else if (isPressing)
        {
            ImVec2 currentWindowSize = ImGui::GetWindowSize();
            mDragSize.x = currentWindowSize.x;
            mDragSize.y = currentWindowSize.y;
		}

        mPressing = isPressing;
    }
}