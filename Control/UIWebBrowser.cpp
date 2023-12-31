#include "StdAfx.h"
#include <atlcomcli.h>
#include <SHLGUID.h>

#include "UIWebBrowser.h"
#include "../Utils/downloadmgr.h"

namespace DuiLib
{

UI_IMPLEMENT_DYNCREATE(CWebBrowserUI);

CWebBrowserUI::CWebBrowserUI()
: m_pWebBrowser2(NULL)
, m_pHtmlDoc(NULL)
, m_glpDisp(NULL)
, m_pHtmlWnd2(NULL)
, m_pDefaultDocHostUIHandler(NULL)
, m_pWebBrowserEventHandler(NULL)
, m_bAutoNavi(false)
, m_dwRef(0)
, m_dwCookie(0)
{
	m_clsid=CLSID_WebBrowser;
	//m_sHomePage.clear();
}

bool CWebBrowserUI::DoCreateControl()
{
	if (!CActiveXUI::DoCreateControl())
		return false;

	GetManager()->AddTranslateAccelerator(this);
	GetControl(IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser2);
	this->Navigate2(_T("about:blank"));
	if ( m_bAutoNavi && !m_sHomePage.empty())
	{
		this->Navigate2(m_sHomePage.c_str());
	}
	RegisterEventHandler(TRUE);
	return true;
}

void CWebBrowserUI::ReleaseControl()
{
	m_bCreated=false;

	GetManager()->RemoveTranslateAccelerator(this);
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->Release();
	}
	RegisterEventHandler(FALSE);
}

CWebBrowserUI::~CWebBrowserUI()
{
	ReleaseControl();
}

STDMETHODIMP CWebBrowserUI::GetTypeInfoCount( UINT *iTInfo )
{
	*iTInfo = 0;
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetIDsOfNames( REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid,DISPID *rgDispId )
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,WORD wFlags, DISPPARAMS* pDispParams,VARIANT* pVarResult, EXCEPINFO* pExcepInfo,UINT* puArgErr )
{
	if ((riid != IID_NULL))
		return E_INVALIDARG;

	switch(dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:
		BeforeNavigate2(
			pDispParams->rgvarg[6].pdispVal,
			pDispParams->rgvarg[5].pvarVal,
			pDispParams->rgvarg[4].pvarVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	case DISPID_COMMANDSTATECHANGE:
		CommandStateChange(
			pDispParams->rgvarg[1].lVal,
			pDispParams->rgvarg[0].boolVal);
		break;
	case DISPID_NAVIGATECOMPLETE2:
		NavigateComplete2(
			pDispParams->rgvarg[1].pdispVal,
			pDispParams->rgvarg[0].pvarVal);
		break;
	case DISPID_NAVIGATEERROR:
		NavigateError(
			pDispParams->rgvarg[4].pdispVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	case DISPID_STATUSTEXTCHANGE:
		break;
		//  	case DISPID_NEWWINDOW2:
		//  		break;
	case DISPID_NEWWINDOW3:
		NewWindow3(
			pDispParams->rgvarg[4].ppdispVal,
			pDispParams->rgvarg[3].pboolVal,
			pDispParams->rgvarg[2].uintVal,
			pDispParams->rgvarg[1].bstrVal,
			pDispParams->rgvarg[0].bstrVal);
		break;
	case DISPID_DOCUMENTCOMPLETE:
		DocumentComplete(
			pDispParams->rgvarg[1].pdispVal,
			pDispParams->rgvarg[0].pvarVal);
		break;
	case DISPID_HTMLELEMENTEVENTS2_ONCLICK:
		OnClick(static_cast<IHTMLEventObj*>(pDispParams->rgvarg[0].pdispVal));
		break;
// 	case DISPID_PROPERTYCHANGE:
// 		if (pDispParams->cArgs>0 && pDispParams->rgvarg[0].vt == VT_BSTR) {
// 			TRACE(_T("PropertyChange(%s)\n"), pDispParams->rgvarg[0].bstrVal);
// 		}
// 		break;
	case DISPID_TITLECHANGE:
		TitleChange(pDispParams->rgvarg[0].bstrVal);
		break;
	default:
		return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWebBrowserUI::QueryInterface( REFIID riid, void **ppvObject )
{
	*ppvObject = NULL;

	if( riid == IID_IDocHostUIHandler)
		*ppvObject = static_cast<IDocHostUIHandler*>(this);
	else if( riid == IID_IDispatch)
		*ppvObject = static_cast<IDispatch*>(this);
	else if( riid == ::IID_IServiceProvider)
		*ppvObject = static_cast<IServiceProvider*>(this);
	else if (riid == IID_IOleCommandTarget)
		*ppvObject = static_cast<IOleCommandTarget*>(this);
	else if (riid == DIID_HTMLDocumentEvents2)
		*ppvObject = static_cast<IDispatch*>(this); 

	if( *ppvObject != NULL )
		AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

ULONG STDMETHODCALLTYPE CWebBrowserUI::AddRef( void )
{
	::InterlockedIncrement(&m_dwRef); 
	return m_dwRef;
}

ULONG STDMETHODCALLTYPE CWebBrowserUI::Release( void )
{
	::InterlockedDecrement(&m_dwRef);
	return m_dwRef;
}

void CWebBrowserUI::Navigate2( LPCTSTR lpszUrl )
{
	if (m_pWebBrowser2 && lpszUrl)
	{
		CDuiVariant url;
		url.vt=VT_BSTR;
		url.bstrVal=T2BSTR(lpszUrl);
		m_pWebBrowser2->Navigate2(&url, NULL, NULL, NULL, NULL);
	}
}

void CWebBrowserUI::Refresh()
{
	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->Refresh();
	}
}

void CWebBrowserUI::GoBack()
{
	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->GoBack();
	}
}

void CWebBrowserUI::GoForward()
{
	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->GoForward();
	}
}

/// DWebBrowserEvents2
void CWebBrowserUI::BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel )
{
	RegisterDocumentEventHandler(FALSE);
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->BeforeNavigate2(pDisp,url,Flags,TargetFrameName,PostData,Headers,Cancel);
	}
}

void CWebBrowserUI::NavigateError( IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->NavigateError(pDisp,url,TargetFrameName,StatusCode,Cancel);
	}
}

void CWebBrowserUI::NavigateComplete2( IDispatch *pDisp,VARIANT *&url )
{
	if (!m_glpDisp)
	{
		m_glpDisp=pDisp;
	}
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->NavigateComplete2(pDisp,url);
	}
}

void CWebBrowserUI::ProgressChange( LONG nProgress, LONG nProgressMax )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->ProgressChange(nProgress,nProgressMax);
	}
}

void CWebBrowserUI::NewWindow3( IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->NewWindow3(pDisp,Cancel,dwFlags,bstrUrlContext,bstrUrl);
	}
}
void CWebBrowserUI::CommandStateChange(long Command,VARIANT_BOOL Enable)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->CommandStateChange(Command,Enable);
	}
}

// IDownloadManager
STDMETHODIMP CWebBrowserUI::Download( /* [in] */ IMoniker *pmk, /* [in] */ IBindCtx *pbc, /* [in] */ DWORD dwBindVerb, /* [in] */ LONG grfBINDF, /* [in] */ BINDINFO *pBindInfo, /* [in] */ LPCOLESTR pszHeaders, /* [in] */ LPCOLESTR pszRedir, /* [in] */ UINT uiCP )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->Download(pmk,pbc,dwBindVerb,grfBINDF,pBindInfo,pszHeaders,pszRedir,uiCP);
	}
	return E_NOTIMPL;
}

// IDocHostUIHandler
STDMETHODIMP CWebBrowserUI::ShowContextMenu( DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->ShowContextMenu(dwID,pptPosition,pCommandTarget,pDispatchObjectHit);
	}
	//返回 E_NOTIMPL 正常弹出系统右键菜单
	//返回S_OK 则可屏蔽系统右键菜单
	return E_NOTIMPL;
	
}

STDMETHODIMP CWebBrowserUI::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetHostInfo(pInfo);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::ShowUI( DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc )
{
	if (m_pDefaultDocHostUIHandler)
	{
		CComPtr<IDocHostUIHandler> pDocHostUIHandler;
		m_pDefaultDocHostUIHandler->QueryInterface(IID_IDocHostUIHandler,(void**)&pDocHostUIHandler);
		if (pDocHostUIHandler)
		{
			return pDocHostUIHandler->ShowUI(dwID,pActiveObject,pCommandTarget,pFrame,pDoc);
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::HideUI()
{
	if (m_pDefaultDocHostUIHandler)
	{
		CComPtr<IDocHostUIHandler> pDocHostUIHandler;
		m_pDefaultDocHostUIHandler->QueryInterface(IID_IDocHostUIHandler,(void**)&pDocHostUIHandler);
		if (pDocHostUIHandler)
		{
			return pDocHostUIHandler->HideUI();
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::UpdateUI()
{
	if (m_pDefaultDocHostUIHandler)
	{
		CComPtr<IDocHostUIHandler> pDocHostUIHandler;
		m_pDefaultDocHostUIHandler->QueryInterface(IID_IDocHostUIHandler,(void**)&pDocHostUIHandler);
		if (pDocHostUIHandler)
		{
			return pDocHostUIHandler->UpdateUI();
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::EnableModeless( BOOL fEnable )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->EnableModeless(fEnable);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::OnDocWindowActivate( BOOL fActivate )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->OnDocWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::OnFrameWindowActivate( BOOL fActivate )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->OnFrameWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->ResizeBorder(prcBorder,pUIWindow,fFrameWindow);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::TranslateAccelerator( LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->TranslateAccelerator(lpMsg,pguidCmdGroup,nCmdID);
	}
	return E_NOTIMPL;
}

LRESULT CWebBrowserUI::TranslateAccelerator( MSG *pMsg )
{
    if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
        return S_FALSE;

	if( m_pWebBrowser2 == NULL )
        return E_NOTIMPL;

    // 当前Web窗口不是焦点,不处理加速键
    BOOL bIsChild = FALSE;
    HWND hTempWnd = NULL;
    HWND hWndFocus = ::GetFocus();

    hTempWnd = hWndFocus;
    while(hTempWnd != NULL)
    {
        if(hTempWnd == m_hwndHost)
        {
            bIsChild = TRUE;
            break;
        }
        hTempWnd = ::GetParent(hTempWnd);
    }
    if(!bIsChild)
        return S_FALSE;

	CComPtr<IOleInPlaceActiveObject> pObj;
	if (FAILED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
		return S_FALSE;

    HRESULT hResult = pObj->TranslateAccelerator(pMsg);
    return hResult;
}

STDMETHODIMP CWebBrowserUI::GetOptionKeyPath( LPOLESTR* pchKey, DWORD dwReserved )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetOptionKeyPath(pchKey,dwReserved);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetDropTarget( IDropTarget* pDropTarget, IDropTarget** ppDropTarget )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetDropTarget(pDropTarget,ppDropTarget);
	}
	return S_FALSE;	// 使用系统拖拽
}

STDMETHODIMP CWebBrowserUI::GetExternal( IDispatch** ppDispatch )
{
	*ppDispatch=NULL;
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetExternal(ppDispatch);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::TranslateUrl( DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->TranslateUrl(dwTranslate,pchURLIn,ppchURLOut);
	}
    else
    {
        *ppchURLOut = 0;
        return E_NOTIMPL;
    }
}

STDMETHODIMP CWebBrowserUI::FilterDataObject( IDataObject* pDO, IDataObject** ppDORet )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->FilterDataObject(pDO,ppDORet);
	}
    else
    {
        *ppDORet = 0;
        return E_NOTIMPL;
    }
}

void CWebBrowserUI::SetWebBrowserEventHandler( CWebBrowserEventHandler* pEventHandler )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->Release();
	}
	m_pWebBrowserEventHandler=pEventHandler;
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->AddRef();
	}
}

void CWebBrowserUI::Refresh2( int Level )
{
	CDuiVariant vLevel;
	vLevel.vt=VT_I4;
	vLevel.intVal=Level;
	m_pWebBrowser2->Refresh2(&vLevel);
}

void CWebBrowserUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if (_tcscmp(pstrName, _T("homepage")) == 0)
	{
		m_sHomePage = pstrValue;
	}
	else if (_tcscmp(pstrName, _T("autonavi"))==0)
	{
		m_bAutoNavi = (_tcscmp(pstrValue, _T("true")) == 0);
	}
	else
		CActiveXUI::SetAttribute(pstrName, pstrValue);
}

void CWebBrowserUI::NavigateHomePage()
{
	if (!m_sHomePage.empty())
		this->Navigate2(m_sHomePage.c_str());
}

LPCTSTR CWebBrowserUI::GetClass() const
{
	return _T("WebBrowserUI");
}

LPVOID CWebBrowserUI::GetInterface( LPCTSTR pstrName )
{
	if( _tcscmp(pstrName, _T("WebBrowser")) == 0 ) return static_cast<CWebBrowserUI*>(this);
	return CActiveXUI::GetInterface(pstrName);
}

void CWebBrowserUI::SetHomePage( LPCTSTR lpszUrl )
{
	m_sHomePage = CDuiStringOperation::format(_T("%s"),lpszUrl);
}

LPCTSTR CWebBrowserUI::GetHomePage()
{
	return m_sHomePage.c_str();
}

void CWebBrowserUI::SetAutoNavigation( bool bAuto /*= TRUE*/ )
{
	if (m_bAutoNavi==bAuto)	return;

	m_bAutoNavi=bAuto;
}

bool CWebBrowserUI::IsAutoNavigation()
{
	return m_bAutoNavi;
}

STDMETHODIMP CWebBrowserUI::QueryService( REFGUID guidService, REFIID riid, void** ppvObject )
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if ( guidService == SID_SDownloadManager && riid == IID_IDownloadManager)
	{
		*ppvObject = this;
		return S_OK;
	}

	return hr;
}

HRESULT CWebBrowserUI::RegisterEventHandler( BOOL inAdvise )
{
	if (m_pWebBrowser2==NULL)
		return S_FALSE;

	HRESULT hr = S_FALSE;
	CComPtr<IConnectionPointContainer>  pCPC;
	CComPtr<IConnectionPoint> pCP;
	
	hr=m_pWebBrowser2->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
	if (FAILED(hr))
		return hr;
	hr=pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
	if (FAILED(hr))
		return hr;

	if (inAdvise)
	{
		hr = pCP->Advise((IDispatch*)this, &m_dwCookie);
	}
	else
	{
		hr = pCP->Unadvise(m_dwCookie);
	}
	return hr; 
}

DISPID CWebBrowserUI::FindId( IDispatch *pObj, LPOLESTR pName )
{
	DISPID id = 0;
	if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
	return id;
}

HRESULT CWebBrowserUI::InvokeMethod( IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs )
{
	DISPID dispid = FindId(pObj, pMehtod);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS dispparams;
	dispparams.cArgs = cArgs;
	dispparams.rgvarg = ps;
	dispparams.cNamedArgs = 0;
	dispparams.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, pVarResult, NULL, NULL);
}

HRESULT CWebBrowserUI::GetProperty( IDispatch *pObj, LPOLESTR pName, VARIANT *pValue )
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
}

HRESULT CWebBrowserUI::SetProperty( IDispatch *pObj, LPOLESTR pName, VARIANT *pValue )
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 1;
	ps.rgvarg = pValue;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
}

IDispatch* CWebBrowserUI::GetHtmlWindow()
{
	CComPtr<IDispatch>	pDp;
	HRESULT hr;
	if (m_pWebBrowser2)
		hr=m_pWebBrowser2->get_Document(&pDp);

	if (FAILED(hr))
		return NULL;

    CComQIPtr<IHTMLDocument2> pHtmlDoc2 = pDp;

	if (pHtmlDoc2 == NULL)
		return NULL;

	hr=pHtmlDoc2->get_parentWindow(&m_pHtmlWnd2);

	if (FAILED(hr))
		return NULL;

	IDispatch *pHtmlWindown = NULL;
	hr=m_pHtmlWnd2->QueryInterface(IID_IDispatch, (void**)&pHtmlWindown);
	if (FAILED(hr))
		return NULL;

	return pHtmlWindown;
}

IWebBrowser2* CWebBrowserUI::GetWebBrowser2( void )
{
	return m_pWebBrowser2;
}

HRESULT STDMETHODCALLTYPE CWebBrowserUI::QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText )
{
	return OLECMDERR_E_NOTSUPPORTED; 
}

HRESULT STDMETHODCALLTYPE CWebBrowserUI::Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut )
{
	HRESULT hr = E_NOTIMPL;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{
		switch (nCmdID) 
		{
		case OLECMDID_SHOWSCRIPTERROR:
			{
				(*pvaOut).vt = VT_BOOL;
				(*pvaOut).boolVal = VARIANT_TRUE;
				return S_OK;
			}
		case OLECMDID_SHOWMESSAGE:
			break;
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}

	return (hr);
}

void CWebBrowserUI::DocumentComplete(IDispatch *pDisp,VARIANT *&url)
{
	CComPtr<IServiceProvider > pISP;
	CComPtr<IServiceProvider> pISPTop;
	CComPtr<IWebBrowser2> pTopTarget;
	pDisp->QueryInterface(::IID_IServiceProvider,(void**)&pISP); //查询服务接口
	pISP->QueryService(SID_STopLevelBrowser, ::IID_IServiceProvider,(void**)&pISPTop); //查询顶层浏览器框架
	pISPTop->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2,(void**)&pTopTarget); //获得顶层框架对象

	if (pDisp == pTopTarget)
	{
		SetUIHandler();
		RegisterDocumentEventHandler(TRUE);
		m_glpDisp = NULL;

		if (m_pWebBrowserEventHandler)
		{
			m_pWebBrowserEventHandler->DocumentComplete(pDisp,url);
		}
	}
}

void CWebBrowserUI::SetUIHandler()
{
	if (m_pDefaultDocHostUIHandler!=NULL)
		return ;
	CComPtr<IDispatch> spDocument;   
	m_pWebBrowser2->get_Document(&spDocument);   

	if (spDocument)
	{
		// Request default handler from MSHTML client site
 		CComPtr<IOleObject> spOleObject;
 		spDocument->QueryInterface(IID_IOleObject,(void**)&spOleObject);
 
 		if (spOleObject)
 		{
 			CComPtr<IOleClientSite> spClientSite;
 			spOleObject->GetClientSite(&spClientSite);
 			if ( spClientSite)
 			{
 				// Save pointer for delegation to default 
 				m_pDefaultDocHostUIHandler = spClientSite;
 			}
 		}

		CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDocument);   
		if (spCustomDoc)
		{
			spCustomDoc->SetUIHandler(this);
		}
	}
}

void CWebBrowserUI::TitleChange( BSTR bstrTitle )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->TitleChange(bstrTitle);
	}
}

HRESULT CWebBrowserUI::RegisterDocumentEventHandler( BOOL inAdvise )
{
	if(!m_pWebBrowser2)
		return S_FALSE;

	DUITRACE(_T("CWebBrowserUI::RegisterDocumentEventHandler"));
	CComPtr<IDispatch> pdispDoc = NULL;
	HRESULT hr = m_pWebBrowser2->get_Document(&pdispDoc);
	if (!pdispDoc)
		return S_FALSE;

	hr = pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_pHtmlDoc);
	if (FAILED(hr))
	{	
		return hr;
	}

	CComPtr<IConnectionPointContainer> pCPC;
	CComPtr<IConnectionPoint> pCP;
	hr = m_pHtmlDoc->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&pCPC);
	if (FAILED(hr))
	{	
		return S_FALSE;
	}

	hr = pCPC->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pCP);
	if (FAILED(hr))
	{
		return S_FALSE;
	}

	if (inAdvise)
	{
		hr = pCP->Advise(static_cast<IDispatch*>(this), &m_dwCookie);
	}
	else
	{
		hr = pCP->Unadvise(m_dwCookie);
	}

	return hr; 
}

VARIANT_BOOL CWebBrowserUI::OnClick( IHTMLEventObj *pEvtObj )
{
	CComPtr<IHTMLElement> pElement;
	pEvtObj->get_srcElement(&pElement); // 事件发生的对象元素
	BSTR bstrElementID;
	HRESULT hRes = pElement->get_id(&bstrElementID);

	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->OnClick((LPCTSTR)bstrElementID,pEvtObj);
	}
	return S_OK;
}

}
