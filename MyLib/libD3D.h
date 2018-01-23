
# include "lib.h"
# include "d3d9.h"
# pragma comment(lib,"d3d9.lib")
# pragma comment(lib,"winmm.lib") 

class lib3D9 : public BaseWindow
{
public:
	bool InitD3D();

	virtual void OnCreate();
	virtual void OnDraw();
	virtual void InitBeforeCreate();
	virtual void AfterCreate();
	virtual void MessageLoop();
	virtual void Display(float delta);

	operator IDirect3D9 *() {return mDirect9;}
	operator IDirect3DDevice9*() {return mDevice9;}

	lib3D9();

	~lib3D9();

private:
	IDirect3D9 * mDirect9;
	IDirect3DDevice9 * mDevice9;
};

