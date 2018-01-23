
# include "lib.h"
# include "d3d9.h"
# pragma comment(lib,"d3d9.lib")
# pragma comment(lib,"winmm.lib") 


class lib3D9 : public BaseWindow
{
public:
	//初始化D3D
	bool InitD3D();

	//释放资源
	void Clearup();

	void Release(void * p);
	void Delete(void * p);

	virtual void OnCreate();
	virtual void OnDraw();
	virtual void InitBeforeCreate();
	virtual void AfterCreate();
	virtual void MessageLoop();
	virtual void Display(float delta);

	operator IDirect3D9 *() {return this->_d3d;}
	operator IDirect3DDevice9*() {return this->Device;}

	friend class BaseWindow;

	//construct
	//Wsq3D():My_Window(){ this->_d3d = NULL; this->Device = NULL;}

	lib3D9();

	//destruct
	~lib3D9();

private:
	IDirect3D9 * _d3d;
	IDirect3DDevice9 * Device;
};

