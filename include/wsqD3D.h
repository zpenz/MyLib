
# include "wsqlib.h"
# include "d3d9.h"
# pragma comment(lib,"d3d9.lib")
# pragma comment(lib,"winmm.lib") 


class Wsq3D : public My_Window
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
	virtual void Init();
	virtual void AfterCreate();
	virtual void MessageLoop();
	virtual void Display(float delta);

	operator IDirect3D9 *() {return this->_d3d;}
	operator IDirect3DDevice9*() {return this->Device;}

	friend class My_Window;

	//construct
	//Wsq3D():My_Window(){ this->_d3d = NULL; this->Device = NULL;}

	Wsq3D();

	//destruct
	~Wsq3D();

private:
	IDirect3D9 * _d3d;
	IDirect3DDevice9 * Device;
};

