#ifndef WindowUI_h__
#define WindowUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	WindowUI.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-11-08 16:11:01
// 说明		:	DirectUI Window
/////////////////////////////////////////////////////////////*/
#pragma once
#include "WindowWnd.h"

namespace DuiLib
{

	class CShadowWindow;
class UILIB_API CWindowUI
	: public CObjectUI
	, public CWindowWnd
	, public INotifyUI
{
	friend class CDialogBuilder;
	friend class CResEngine;
public:
	CWindowUI(void);
	virtual ~CWindowUI(void);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	CControlUI *GetRoot() const;
	CControlUI *GetItem(LPCTSTR lpszItemPath) const;

	CControlUI* FindControl(POINT pt) const;
	CControlUI* FindControl(LPCTSTR pstrName) const;
	CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;

	virtual void SetWindowTemplate(LPCTSTR lpszTemplateName);
	virtual LPCTSTR GetWindowTemplate();

	virtual bool Notify(TNotifyUI *pMsg);

	HWND CreateDuiWindow(HWND hwndParent, LPCTSTR lpszWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);

	virtual UINT DoModal();
	bool IsModal();

	bool ShowWindow(int nCmdShow = SW_SHOW);

	POINT GetMousePos() const;

	// 窗口属性接口
	SIZE GetInitSize();
	void SetInitSize(int cx, int cy);
	RECT GetSizeBox();
	void SetSizeBox(RECT& rcSizeBox);
	RECT GetCaptionRect();
	void SetCaptionRect(RECT& rcCaption);
	void SetShadowEnable(bool bEnable = true);

	// 焦点
	CControlUI* GetFocus() const;
	void SetFocus(CControlUI* pControl);
	void SetFocusNeeded(CControlUI* pControl);

	// 设置和获取窗口Capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	// 刷新
	bool IsUpdateNeeded() const;
	void NeedUpdate();
	void Invalidate();
	void Invalidate(RECT& rcItem);

	void SetAnimating(bool bAnimating);
	bool GetAnimating();
	void SetLayerWindow(bool bLayerWindow,bool bRedraw);
	bool IsLayerWindow();

	// title
	void SetWindowTitle(LPCTSTR lpszWindowTitle);
	LPCTSTR	GetWindowTitle();

	bool SetNextTabControl(bool bForward = true);

	// 窗口控件消息通知接口
	void AddNotify(INotifyUI *pNotify);
	void RemoveNotify(INotifyUI *pNotify);

	void SendNotify(TNotifyUI *pMsg, bool bAsync = false);
	void SendNotify(CControlUI* pControl, UINOTIFY dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
	void AddDelayedCleanup(CControlUI* pControl);
	void ReapObjects(CControlUI* pControl);

	bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);
	void KillTimer(CControlUI* pControl);
	void RemoveAllTimers();

	HDC GetPaintDC();

	int GetPostPaintCount() const;
	bool AddPostPaint(CControlUI* pControl);
	bool RemovePostPaint(CControlUI* pControl);
	bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

	bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
	void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	void RemoveAllOptionGroups();

public:
	// 当前窗口的消息过滤
	void AddWindowMessageFilter(IMessageFilterUI* pFilter);
	void RemoveWindowMessageFilter(IMessageFilterUI* pFilter);

	// 全局消息的过滤
	void AddPreMessageFilter(IMessageFilterUI* pFilter);
	void RemovePreMessageFilter(IMessageFilterUI* pFilter);

	// 加速键消息预处理
	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);

public:
	// 实现自绘窗口的窗口消息处理
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	// 重写的CWindowWnd窗口消息循环
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT CustomMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual void SendNotifyEvent(TNotifyUI *pMsg);

public:
	// 优先过滤消息
	virtual bool PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);
	// 处理加速键消息
	virtual bool TranslateAccelerator(MSG *pMsg);

	LRESULT ReflectNotifications(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ bool& bHandled);

protected:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;

protected:
	// 控件与实例
	HINSTANCE m_hInstance;
	CControlUI *m_pRootControl;
	CControlUI *m_pEventClick;
	CControlUI *m_pEventHover;
	CControlUI *m_pFocus;
	CControlUI *m_pEventKey;

	// 窗口相关
	CDuiRect m_rcRestore;
	CDuiSize m_szRoundCorner;		
	CDuiSize m_szMinWindow;				// 窗口最小
	CDuiSize m_szMaxWindow;				// 窗口最大
	CDuiSize m_szInitWindowSize;	// 初始化窗口大小
	CDuiRect m_rcSizeBox;				// 可调整边框边距
	CDuiRect m_rcCaption;				// 响应标题栏拖动

	CDuiString m_strTemplate;

	// 刷新
	bool m_bShowUpdateRect;
	BYTE m_nAlpha;
	bool m_bLayedWindow;
	bool m_bAnimating;
	bool m_bLayerWindow;
	CDuiString m_strWindowTitle;

	// 布局与渲染
	bool m_bFirstLayout;
	bool m_bUpdateNeeded;
	bool m_bFocusNeeded;
	HDC m_hPaintDC;
	CMemDC m_OffscreenDC;
	HPEN m_hUpdateRectPen;

	// Shadow
	bool m_bShowShadowBorder;
	CShadowWindow *m_pShadowWindow;

	// Tooltip
	HWND m_hWndTooltip;
	TOOLINFO m_ToolTip;

	// Events
	CDuiPoint m_ptLastMousePos;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	CStdPtrArray m_arrayDelayedCleanup;
	CStdPtrArray m_arrayPostPaintControls;
	CStdPtrArray m_arrayAsyncNotify;
	CStdPtrArray m_arrayFoundedControls;
	CStdPtrArray m_arrayNotifyFilters;
	CStdPtrArray m_arrayWindowMessageFilters;
	CStdPtrArray m_arrayPreMessageFilters;
	CStdPtrArray m_arrayTranslateAccelerators;
	CStdPtrArray m_arrayTimers;
	UINT m_uTimerID;

	CStdStringPtrMap m_mapNameHash;
	CStdStringPtrMap m_mapOptionGroup;

#ifdef _DEBUG
	void TestUICrossThread();
#endif // _DEBUG

private:
	CStdPtrArray* GetFoundControls();
	static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlsFromUpdate(CControlUI* pThis, LPVOID pData);
};

}
#endif // WindowUI_h__
