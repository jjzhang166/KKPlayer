#ifndef IkkDraw_H_
#define IkkDraw_H_
/**
*绘制接口，用以兼容skia,gdi+,opengl,dx等，本不想提供这些的，但被别人鄙视了，就只好提供出来
*/
//一些宏定义
#define kkRGB(r,g,b)              ((unsigned long )(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
#define kkMax(a,b)                ((a) > (b) ? (a) : (b))
#define kkMin(a,b)                ((a) < (b) ? (a) : (b))
#define kkMakeUint(a, b)          ((unsigned short)(((unsigned char)(((unsigned long)(a)) & 0xff)) | ((unsigned short)((unsigned char)(((unsigned long)(b)) & 0xff))) << 8))
struct kkPoint
{
	int x;
	int y;
};
static inline bool  operator == (const kkPoint& Other1, const kkPoint& Other2)
{
	if (Other1.y == Other2.y&&Other1.x == Other2.x)
		return true;
	
	return false;
}
static inline bool    operator > (const kkPoint& Other1, const  kkPoint& Other2)
{
	if (Other1.y > Other2.y)
		return true;
	else if (Other1.y == Other2.y &&Other1.x > Other2.x)
		return true;
	return false;
}
static inline bool    operator < (const kkPoint& Other1, const kkPoint& Other2)
{
	if (Other1.y < Other2.y)
		return true;
	else if (Other1.y == Other2.y&&Other1.x < Other2.x)
		return true;
	return false;
}
typedef struct kkRect
{
	int left;
	int top;
	int right;
	int bottom;
}kkRect;



/**
*根据两点生成一个矩形
*/
static inline void GetkkRectByPoint(kkPoint p1, kkPoint p2, kkRect& OutRt)
{
	if (p1.x == p2.x)
	    p1.x += 1;
	if (p1.y == p2.y)
		p2.y += 1;
	OutRt = { kkMin(p1.x, p2.x), kkMin(p1.y, p2.y), kkMax(p1.x, p2.x), kkMax(p1.y, p2.y) };
}
static inline void kkRectOutset(kkRect& OutRt, int x, int y)
{
	OutRt.left -= x;
	OutRt.top -= y;
	OutRt.right += x;
	OutRt.bottom += y;
}
/**
*判断矩形是否相交
*/
static inline bool kkRectIntersects(kkRect rt1,kkRect rt2) {
	int L = kkMax(rt1.left,rt2.left);
	int R = kkMin(rt1.right,rt2.right);
	int T = kkMax(rt1.top, rt2.top);
	int B = kkMin(rt1.bottom,rt2.bottom);
	return L < R && T < B;
}
/**
*合并两个矩形
*/
static inline void kkRectMerge(kkRect &rt1, const kkRect &rt2) {
	int L = kkMin(rt1.left, rt2.left);
	int T =  kkMin(rt1.top, rt2.top);
	int R = kkMax(rt1.right, rt2.right);
	int B =kkMax(rt1.bottom, rt2.bottom);
	rt1 = {L,T,R,B};
}

/**
*判断点是否在矩形区域内
*/
static inline bool kkPtInRect(const kkPoint &pt, const kkRect& rt){
	return  ((unsigned)(pt.x - rt.left) <(unsigned)(rt.right - rt.left)) && ((unsigned)(pt.y - rt.top) < (unsigned)(rt.bottom - rt.top));
}
static inline void kkGetRectWH(const kkRect& rt,int &w,int &h){
	w = rt.right - rt.left;
	h = rt.bottom - rt.top;
}
enum kkStrokeStyle{
	kkFill_Style=0,          ///填充模式
	kkPen_Style=1,           ///画笔模式
};

#endif