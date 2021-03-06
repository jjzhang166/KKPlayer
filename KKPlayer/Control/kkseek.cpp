#include "kkseek.h"
namespace SOUI
{

#define TIMERID_NOTIFY1     1
#define TIMERID_DELAY1      2

//////////////////////////////////////////////////////////////////////////
//  SAVSeekBar
SAVSeekBar::SAVSeekBar()
    : m_bDrag(FALSE)
    , m_uHtPrev(-1)
    , m_pSkinThumb(GETBUILTINSKIN(SKIN_SYS_SLIDER_THUMB))
    , m_bThumbInRail(FALSE)
	, m_nCacheValue(0)
	, m_pSkinCachePos(NULL)
{
    m_evtSet.addEvent(EVENTID(EventSliderPos));
}

SAVSeekBar::~SAVSeekBar()
{
}

BOOL SAVSeekBar::SetCacheValue(int nValue)
{
	if(nValue<m_nValue) 
		nValue =m_nValue;
	if(nValue>m_nMaxValue) 
		nValue=m_nMaxValue;

	m_nCacheValue=nValue;
	Invalidate();
	return TRUE;
}
int SAVSeekBar::HitTest(CPoint pt)
{
    CRect rc;

    rc = GetPartRect(SC_THUMB);
    if (rc.PtInRect(pt))
        return SC_THUMB;

    rc = GetPartRect(SC_SELECT);
    if (rc.PtInRect(pt))
        return SC_SELECT;

    rc = GetPartRect(SC_RAIL);
    if (rc.PtInRect(pt))
        return SC_RAIL;

    return -1;
}


SAVSeekBar::RANGE SAVSeekBar::_GetPartRange( int nLength,int nThumbSize, BOOL bThumbInRail, int nMin,int nMax,int nValue, UINT uSBCode )
{
    int nRailLen = nLength - nThumbSize;
    
    int nHalfThumb = nThumbSize/2;
    int nSelect = nRailLen*(nValue-nMin)/(nMax-nMin);

    RANGE rRet={0};
    switch(uSBCode)
    {
    case SC_RAILBACK://轨道背景
        rRet.value1=0;
        rRet.value2=nLength;
        if(!bThumbInRail)
        {
            rRet.value1+=nHalfThumb;
            rRet.value2-=nHalfThumb;
        }
        break;
    case SC_RAIL:   //轨道区
        rRet.value1=0;
        rRet.value2=nLength;
        rRet.value1+=nHalfThumb;
        rRet.value2-=nHalfThumb;
        break;
    case SC_SELECT:
        rRet.value1=0;
        rRet.value2=nSelect;
        if(!bThumbInRail)
        {
            rRet.value1+=nHalfThumb;
        }
        rRet.value2+=nHalfThumb;
        break;
    case SC_THUMB:
        rRet.value1 = nSelect;
        rRet.value2 = nSelect+nThumbSize;
        break;
    }

    return rRet;
}


CRect SAVSeekBar::GetPartRect(UINT uSBCode)
{
    SASSERT(m_pSkinThumb);

    CRect rcClient;
    GetClientRect(&rcClient);
    
    SIZE szThumb = m_pSkinThumb->GetSkinSize();
    SIZE szRail  = m_pSkinBg->GetSkinSize();

    if(IsVertical())
    {
        RANGE r = _GetPartRange(rcClient.Height(),szThumb.cy,m_bThumbInRail,m_nMinValue,m_nMaxValue,m_nValue,uSBCode);
        CRect rc(rcClient.left,r.value1,rcClient.right,r.value2);
        rc.OffsetRect(0,rcClient.top);
        int   nSliderSize = max(szThumb.cx,szRail.cx);
        rc.DeflateRect((rcClient.Width()-nSliderSize)/2,0);
        if(uSBCode != SC_THUMB)
        {
            rc.DeflateRect((nSliderSize-szRail.cx)/2,0);
        }
        return rc;
    }else
    {
        RANGE r = _GetPartRange(rcClient.Width(),szThumb.cx,m_bThumbInRail,m_nMinValue,m_nMaxValue,m_nValue,uSBCode);
        CRect rc(r.value1,rcClient.top,r.value2,rcClient.bottom);
        rc.OffsetRect(rcClient.left,0);
        int   nSliderSize = max(szThumb.cy,szRail.cy);
        rc.DeflateRect(0,(rcClient.Height()-nSliderSize)/2);
        if(uSBCode != SC_THUMB)
        {
            rc.DeflateRect(0,(nSliderSize-szRail.cy)/2);
        }
        return rc;
    }
}

void SAVSeekBar::OnPaint(IRenderTarget * pRT)
{
    SASSERT(m_pSkinThumb && m_pSkinBg && m_pSkinPos);

    SPainter painter;

    BeforePaint(pRT, painter);



	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcValue=rcClient;

	CRect rcThumb = GetPartRect(SC_THUMB);
	int tw=(rcThumb.right-rcThumb.left);
	if(IsVertical()){
		rcValue.bottom=rcValue.top+(int)(((__int64)rcValue.Height())*(m_nCacheValue-m_nMinValue)/(__int64)(m_nMaxValue-m_nMinValue));
		if(rcThumb.bottom>rcValue.bottom&&m_nCacheValue>m_nValue){
           rcValue.bottom+=tw;
		}
	}else{
		rcValue.right=rcValue.left+(int)(((__int64)rcValue.Width())*(m_nCacheValue-m_nMinValue)/(__int64)(m_nMaxValue-m_nMinValue));
		if(rcThumb.right>rcValue.right&&m_nCacheValue>m_nValue){
			rcValue.right+=tw;
		}
	}
	
	

    CRect rcRail=rcClient;//GetPartRect(SC_RAILBACK);
	m_pSkinBg->Draw(pRT,rcRail,0);
	
	if(m_pSkinCachePos!=NULL){
		if(m_nCacheValue>m_nValue)
			m_pSkinCachePos->Draw(pRT, rcValue, 0);
	}
	int selectwidth=0;
    if(m_nValue!=m_nMinValue)
    {
        CRect rcSel=GetPartRect(SC_SELECT);
		rcSel.left=rcClient.left;
		selectwidth=rcSel.right-rcSel.left;
        m_pSkinPos->Draw(pRT,rcSel,0);
    }

	

  
    int nState=0;//normal
    if(m_bDrag) 
		nState=2;//pushback
    else if(m_uHtPrev==SC_THUMB) 
		nState=1;//hover
    
	tw=tw/2;
	if(nState==0&&selectwidth<=tw){
        

	}else{
        m_pSkinThumb->Draw(pRT, rcThumb, nState);
	}
	

    AfterPaint(pRT, painter);
}

void SAVSeekBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();

    if (m_bDrag)
    {
        m_bDrag   = FALSE;
        CRect rcThumb = GetPartRect(SC_THUMB);
        InvalidateRect(rcThumb);
    }
    OnMouseMove(nFlags,point);
}

void SAVSeekBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetCapture();

    UINT uHit = HitTest(point);
    if (uHit == SC_THUMB)
    {
        m_bDrag    = TRUE;
        m_ptDrag   = point;
        m_nDragValue=m_nValue;
        Invalidate();
    }
    else
    {
        CRect rcRail=GetPartRect(SC_RAIL);
        int nValue=0;
        if(IsVertical())
        {
            nValue= (point.y-rcRail.top)*(m_nMaxValue-m_nMinValue+1)/rcRail.Height()+m_nMinValue;
        }else
        {
            nValue= (point.x-rcRail.left)*(m_nMaxValue-m_nMinValue+1)/rcRail.Width()+m_nMinValue;
        }
        SetValue(nValue);
        NotifyPos(SC_THUMB,m_nValue);
        Invalidate();

        m_bDrag    = TRUE;
        m_ptDrag   = point;
        m_nDragValue=m_nValue;
    }
}

void SAVSeekBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (m_bDrag)
    {
        CRect rcRail=GetPartRect(SC_RAIL);

        int nInterHei=(IsVertical()?rcRail.Height():rcRail.Width());
        int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
        int nSlide=nDragLen*(m_nMaxValue-m_nMinValue+1)/nInterHei;

        int nNewTrackPos=m_nDragValue+nSlide;
        if(nNewTrackPos<m_nMinValue)
        {
            nNewTrackPos=m_nMinValue;
        }
        else if(nNewTrackPos>m_nMaxValue)
        {
            nNewTrackPos=m_nMaxValue;
        }
        if(nNewTrackPos!=m_nValue)
        {
            m_nValue=nNewTrackPos;
            Invalidate();
            NotifyPos(SC_THUMB,m_nValue);
        }
    }
    else
    {
        int uHit = HitTest(point);
        if (uHit != m_uHtPrev && (m_uHtPrev==SC_THUMB || uHit==SC_THUMB))
        {
            m_uHtPrev = uHit;
            CRect rcThumb = GetPartRect(SC_THUMB);
            InvalidateRect(rcThumb);
        }
    }
}

void SAVSeekBar::OnMouseLeave()
{
    if (!m_bDrag && m_uHtPrev==SC_THUMB)
    {
        CRect rcThumb = GetPartRect(SC_THUMB);
        InvalidateRect(rcThumb);
        Invalidate();
        m_uHtPrev=-1;
    }
}

LRESULT SAVSeekBar::NotifyPos(UINT uCode, int nPos)
{
    EventSliderPos evt(this);
    evt.nPos = nPos;

    return FireEvent(evt);
}

CSize SAVSeekBar::GetDesiredSize(LPCRECT pRcContainer)
{
    SASSERT(m_pSkinBg && m_pSkinThumb);
    CSize szRet;
    SIZE sizeBg = m_pSkinBg->GetSkinSize();
    SIZE sizeThumb= m_pSkinThumb->GetSkinSize();
    
    if(IsVertical())
    {
        szRet.cx=max(sizeBg.cx,sizeThumb.cx);
        szRet.cy=100;
    }else
    {
        szRet.cy=max(sizeBg.cy,sizeThumb.cy);
        szRet.cx=100;
    }
    return szRet;
}

void SAVSeekBar::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkinThumb) m_pSkinThumb->OnColorize(cr);
}

}//end of namespace