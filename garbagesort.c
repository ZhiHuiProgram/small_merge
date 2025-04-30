#include "Python.h"

void pythoninit(){
	Py_Initialize();
	PyObject *sys = PyImport_ImportModule("sys");//sys = import sys
	PyObject *path = PyObject_GetAttrString(sys, "path");//path = sys.path
	PyList_Append(path, PyUnicode_FromString("/home/orangepi/yuanxueshe/garbagesort"));//将当前路径添加到python路径
}
void pythonfinal(){
	Py_Finalize();

}
char *garbagesort(char *model, char *function){
	char *ret;
	PyObject *pModule = PyImport_ImportModule(model);//导入nopara模块，返回空为失败
	if(!pModule){
		PyErr_Print();
		goto FAILED_MODULE;
	}
	PyObject *pFunc = PyObject_GetAttrString(pModule, function);
	if (!pFunc)
	{
		PyErr_Print();
		printf("Error: failed to load alibabacloud_garbage\n");
		goto FAILED_FUNC;
	}
	PyObject *pValue = PyObject_CallObject(pFunc,NULL);//检查pfunc是否调用成功，把pargs传给pfunc返回ture false
	if (!pValue)
	{
		PyErr_Print();
		printf("Error: function call failed\n");
		goto FAILED_VALUE;
	}	
	if (!PyArg_Parse(pValue, "s", &ret))
	{
		PyErr_Print();
		printf("Error: parse failed");
		goto FAILED_RESULT;
	}
													   
	char *ret_garbage;
	ret_garbage =(char *)malloc(sizeof(ret)+1);
	strcpy(ret_garbage,ret);
FAILED_RESULT:
	Py_DECREF(pValue);
FAILED_VALUE:
	Py_DECREF(pFunc);	
FAILED_FUNC:
	Py_DECREF(pModule);//释放引入的模块，后引入的先释放
FAILED_MODULE:
	return ret_garbage;
// 无参数
//	Py_Initialize();
//       PyObject *sys = PyImport_ImportModule("sys");//sys = import sys
//       PyObject *path = PyObject_GetAttrString(sys, "path");//path = sys.path
//       PyRun_SimpleString("print('sys.path')");//打印sys.path
//       PyList_Append(path, PyUnicode_FromString("."));//将当前路径添加到python路径
//  
//      PyObject *pModule = PyImport_ImportModule("python");//导入nopara模块，返回空为失败
//      if(!pModule){
//          PyErr_Print();
//      }
//      PyObject *pFunc = PyObject_GetAttrString(pModule, "mfun");
//      PyObject *pValue = PyObject_CallObject(pFunc, NULL);//检查pfunc是否调用成功，返回ture false
//      Py_DECREF(pValue);
//      Py_DECREF(pFunc);
//      Py_DECREF(pModule);//释放引入的模块，后引入的先释放
//

}
