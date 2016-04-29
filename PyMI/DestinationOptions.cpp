#include "stdafx.h"
#include "DestinationOptions.h"
#include "PyMI.h"
#include "Utils.h"


static PyObject* DestinationOptions_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DestinationOptions* self = NULL;
    sizeof(DestinationOptions);
    self = (DestinationOptions*)type->tp_alloc(type, 0);
    self->destinationOptions = NULL;
    ::InitializeCriticalSection(&self->cs);
    return (PyObject *)self;
}

static int DestinationOptions_init(DestinationOptions* self, PyObject* args, PyObject* kwds)
{
    PyErr_SetString(PyMIError, "An DestinationOptions object cannot be allocated directly.");
    return -1;
}

static void DestinationOptions_dealloc(DestinationOptions* self)
{
    AllowThreads(&self->cs, [&]() {
        self->destinationOptions = NULL;
    });
    ::DeleteCriticalSection(&self->cs);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

DestinationOptions* DestinationOptions_New(std::shared_ptr<MI::DestinationOptions> destinationOptions)
{
    DestinationOptions* obj = (DestinationOptions*)DestinationOptions_new(&DestinationOptionsType, NULL, NULL);
    obj->destinationOptions = destinationOptions;
    return obj;
}

static PyObject* DestinationOptions_Clone(DestinationOptions *self, PyObject*)
{
    try
    {
        std::shared_ptr<MI::DestinationOptions> destinationOptions;
        AllowThreads(&self->cs, [&]() {
            destinationOptions = self->destinationOptions->Clone();
        });
        return (PyObject*)DestinationOptions_New(destinationOptions);
    }
    catch (std::exception& ex)
    {
        SetPyException(ex);
        return NULL;
    }
}

static PyObject* DestinationOptions_GetUILocale(DestinationOptions* self)
{
    try
    {
        std::wstring locale;
        AllowThreads(&self->cs, [&]() {
            locale = self->destinationOptions->GetUILocale();
        });
        const std::string sTmp(locale.begin(), locale.end());
        return PyUnicode_FromString(sTmp.c_str());
    }
    catch (std::exception& ex)
    {
        SetPyException(ex);
        return NULL;
    }
}

static PyObject* DestinationOptions_SetUILocale(DestinationOptions* self, PyObject *args, PyObject *kwds)
{
    wchar_t* locale = NULL;
    static char *kwlist[] = { "locale_name", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "u", kwlist, &locale))
        return NULL;

    try
    {
        AllowThreads(&self->cs, [&]() {
            self->destinationOptions->SetUILocale(locale);
        });
        Py_RETURN_NONE;
    }
    catch (std::exception& ex)
    {
        SetPyException(ex);
        return NULL;
    }
}

static PyObject* DestinationOptions_SetImpersonationType(DestinationOptions* self, PyObject *args, PyObject *kwds)
{
    wchar_t* impersonationLevel = NULL;
    MI_DestinationOptions_ImpersonationType impersonationType = MI_DestinationOptions_ImpersonationType_None;
    static wchar_t *impersEnum[] = {L"default", L"none", L"identify", L"impersonate", L"delegate"};
    static char *kwlist[] = { "impersonationLevel", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "u", kwlist, &impersonationLevel))
        return NULL;

    for(int i=0; i<=MI_DestinationOptions_ImpersonationType_Delegate; i++)
    {
               if (!wcscmp(impersEnum[i], impersonationLevel))
               {
                       impersonationType = MI_DestinationOptions_ImpersonationType(i);
                       break;
               }
    }
    try
    {
        AllowThreads(&self->cs, [&]() {
            self->destinationOptions->SetImpersonationType(impersonationType);
        });
        Py_RETURN_NONE;
    }
    catch (std::exception& ex)
    {
        SetPyException(ex);
        return NULL;
    }
}


static PyMemberDef DestinationOptions_members[] = {
    { NULL }  /* Sentinel */
};

static PyMethodDef DestinationOptions_methods[] = {
    { "clone", (PyCFunction)DestinationOptions_Clone, METH_NOARGS, "Clones the DestinationOptions." },
    { "get_ui_locale", (PyCFunction)DestinationOptions_GetUILocale, METH_NOARGS, "Returns the UI locale." },
    { "set_ui_locale", (PyCFunction)DestinationOptions_SetUILocale, METH_VARARGS | METH_KEYWORDS, "Sets the UI locale." },
    { "set_impersonation_level", (PyCFunction)DestinationOptions_SetImpersonationType, METH_VARARGS | METH_KEYWORDS, "Sets the impersonation level." },
    { NULL }  /* Sentinel */
};

PyTypeObject DestinationOptionsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "mi.destinationoptions",     /*tp_name*/
    sizeof(DestinationOptions),  /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DestinationOptions_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DestinationOptions objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    DestinationOptions_methods,             /* tp_methods */
    DestinationOptions_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)DestinationOptions_init,    /* tp_init */
    0,                         /* tp_alloc */
    DestinationOptions_new,                 /* tp_new */
};
