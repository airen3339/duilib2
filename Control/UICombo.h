#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;
class CEditWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();
	UI_DECLARE_DYNCREATE();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
	virtual bool IsClientArea();

    void DoInit();
    UINT GetControlFlags() const;

    LPCTSTR GetText() const;
    void SetEnabled(bool bEnable = true);

    LPCTSTR GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);

    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    bool Activate();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    TListInfoUI* GetListInfo();
    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
	DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void EventHandler(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    CComboWnd*	m_pWindow;
	CEditWnd*		m_pEditWnd;

    int m_iCurSel;					// 选中项索引序号
    RECT m_rcTextPadding;	// 文字区
    CDuiString m_sDropBoxAttributes;	// 下拉框属性
    SIZE m_szDropBox;			// 下拉框高度
    UINT m_uButtonState;		// 按钮状态

    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;

    TListInfoUI m_ListInfo;
};

} // namespace DuiLib

#endif // __UICOMBO_H__
