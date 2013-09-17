#include <Python.h>
#include <structmember.h>
#include <libvex.h>

#include "pyvex_types.h"
#include "pyvex_macros.h"
#include "vex/angr_vexir.h"

//////////////////////////
// IRStmtTag translator //
//////////////////////////

static const char *IRStmtTag_to_str(IRStmtTag t)
{
	switch (t)
	{
		PYVEX_ENUMCONV_TOSTRCASE(Ist_NoOp)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_IMark)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_AbiHint)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_Put)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_PutI)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_WrTmp)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_Store)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_CAS)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_LLSC)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_Dirty)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_MBE)
		PYVEX_ENUMCONV_TOSTRCASE(Ist_Exit)
		default:
			fprintf(stderr, "PyVEX: Unknown IRStmtTag");
			return NULL;
	}
}

static IRStmtTag str_to_IRStmtTag(const char *s)
{
	PYVEX_ENUMCONV_FROMSTR(Ist_NoOp)
	PYVEX_ENUMCONV_FROMSTR(Ist_IMark)
	PYVEX_ENUMCONV_FROMSTR(Ist_AbiHint)
	PYVEX_ENUMCONV_FROMSTR(Ist_Put)
	PYVEX_ENUMCONV_FROMSTR(Ist_PutI)
	PYVEX_ENUMCONV_FROMSTR(Ist_WrTmp)
	PYVEX_ENUMCONV_FROMSTR(Ist_Store)
	PYVEX_ENUMCONV_FROMSTR(Ist_CAS)
	PYVEX_ENUMCONV_FROMSTR(Ist_LLSC)
	PYVEX_ENUMCONV_FROMSTR(Ist_Dirty)
	PYVEX_ENUMCONV_FROMSTR(Ist_MBE)
	PYVEX_ENUMCONV_FROMSTR(Ist_Exit)

	return 0;
}

///////////////////////
// IRStmt base class //
///////////////////////

PYVEX_NEW(IRStmt)
PYVEX_DEALLOC(IRStmt)
PYVEX_METH_STANDARD(IRStmt)

static int
pyIRStmt_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);
	PyErr_SetString(VexException, "Base IRStmt creation not supported.");
	return -1;
}

PYVEX_SETTER(IRStmt, wrapped)
PYVEX_GETTER(IRStmt, wrapped)
PYVEX_GETTAG(IRStmt)
PYVEX_SETTAG(IRStmt)

static PyGetSetDef pyIRStmt_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmt, wrapped),
	PYVEX_ACCESSOR_DEF(IRStmt, tag),
	{NULL}
};

static PyObject *pyIRStmt_flat(pyIRStmt* self)
{
	if (isFlatIRStmt(self->wrapped)) { Py_RETURN_TRUE; }
	Py_RETURN_FALSE;
}

static PyMethodDef pyIRStmt_methods[] =
{
	PYVEX_METHDEF_STANDARD(IRStmt),
	{"flat", (PyCFunction)pyIRStmt_flat, METH_NOARGS, "Returns true if IRStmt is flat, false otherwise."},
	{NULL}
};

static PyMemberDef pyIRStmt_members[] = { {NULL} };
PYVEX_TYPEOBJECT(IRStmt);

// wrap functionality
PyObject *wrap_IRStmt(IRStmt *i)
{
	PyTypeObject *t = NULL;

	switch (i->tag)
	{
		PYVEX_WRAPCASE(IRStmt, Ist, NoOp)
		PYVEX_WRAPCASE(IRStmt, Ist, IMark)
		PYVEX_WRAPCASE(IRStmt, Ist, AbiHint)
		PYVEX_WRAPCASE(IRStmt, Ist, Put)
		//PYVEX_WRAPCASE(IRStmt, Ist, PutI)
		PYVEX_WRAPCASE(IRStmt, Ist, WrTmp)
		//PYVEX_WRAPCASE(IRStmt, Ist, Store)
		//PYVEX_WRAPCASE(IRStmt, Ist, CAS)
		//PYVEX_WRAPCASE(IRStmt, Ist, LLSC)
		//PYVEX_WRAPCASE(IRStmt, Ist, Dirty)
		//PYVEX_WRAPCASE(IRStmt, Ist, MBE)
		//PYVEX_WRAPCASE(IRStmt, Ist, Exit)
		default:
			fprintf(stderr, "PyVEX: Unknown/unsupported IRStmtTag %s\n", IRStmtTag_to_str(i->tag));
			t = &pyIRStmtType;
	}

	PyObject *args = Py_BuildValue("");
	PyObject *kwargs = Py_BuildValue("{s:O}", "wrap", PyCapsule_New(i, "IRStmt", NULL));
	PyObject *o = PyObject_Call((PyObject *)t, args, kwargs);
	Py_DECREF(args); Py_DECREF(kwargs);
	return (PyObject *)o;
}

/////////////////
// NoOp IRStmt //
/////////////////

static int
pyIRStmtNoOp_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	if (!kwargs) { self->wrapped = IRStmt_NoOp(); return 0; }
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	PyErr_SetString(VexException, "Unexpected arguments provided to constructor.");
	return -1;
}

static PyMethodDef pyIRStmtNoOp_methods[] = { {NULL} };
static PyGetSetDef pyIRStmtNoOp_getseters[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRStmtNoOp, IRStmt);

//////////////////
// IMark IRStmt //
//////////////////

static int
pyIRStmtIMark_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	Addr64 addr = 0;
	Int len = 0;
	UChar delta = 0;

	static char *kwlist[] = {"addr", "len", "delta", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Kib|O", kwlist, &addr, &len, &delta, &wrap_object)) return -1;

	self->wrapped = IRStmt_IMark(addr, len, delta);
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, wrapped->Ist.IMark.addr, addr, "K")
PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, wrapped->Ist.IMark.len, len, "i")
PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, wrapped->Ist.IMark.delta, delta, "b")

static PyGetSetDef pyIRStmtIMark_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtIMark, addr),
	PYVEX_ACCESSOR_DEF(IRStmtIMark, len),
	PYVEX_ACCESSOR_DEF(IRStmtIMark, delta),
	{NULL}
};

static PyMethodDef pyIRStmtIMark_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRStmtIMark, IRStmt);

////////////////////
// AbiHint IRStmt //
////////////////////

static int
pyIRStmtAbiHint_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	pyIRExpr *base;
	Int len = 0;
	pyIRExpr *nia;

	static char *kwlist[] = {"base", "len", "nia", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OiO|O", kwlist, &base, &len, &nia, &wrap_object)) return -1;
	PYVEX_CHECKTYPE(base, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(nia, pyIRExprType, return -1)

	self->wrapped = IRStmt_AbiHint(base->wrapped, len, nia->wrapped);
	return 0;
}

PYVEX_ACCESSOR_WRAPPED(IRStmtAbiHint, IRStmt, wrapped->Ist.AbiHint.base, base, IRExpr)
PYVEX_ACCESSOR_BUILDVAL(IRStmtAbiHint, IRStmt, wrapped->Ist.AbiHint.len, len, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtAbiHint, IRStmt, wrapped->Ist.AbiHint.nia, nia, IRExpr)

static PyGetSetDef pyIRStmtAbiHint_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, base),
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, len),
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, nia),
	{NULL}
};

static PyMethodDef pyIRStmtAbiHint_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRStmtAbiHint, IRStmt);

////////////////
// Put IRStmt //
////////////////

static int
pyIRStmtPut_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	Int offset = 0;
	pyIRExpr *data;

	static char *kwlist[] = {"offset", "data", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO|O", kwlist, &offset, &data, &wrap_object)) return -1;
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)

	self->wrapped = IRStmt_Put(offset, data->wrapped);
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtPut, IRStmt, wrapped->Ist.Put.offset, offset, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtPut, IRStmt, wrapped->Ist.Put.data, data, IRExpr)

static PyGetSetDef pyIRStmtPut_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtPut, offset),
	PYVEX_ACCESSOR_DEF(IRStmtPut, data),
	{NULL}
};

static PyMethodDef pyIRStmtPut_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRStmtPut, IRStmt);

//////////////////
// WrTmp IRStmt //
//////////////////

static int
pyIRStmtWrTmp_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	Int tmp = 0;
	pyIRExpr *data;

	static char *kwlist[] = {"tmp", "data", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO|O", kwlist, &tmp, &data, &wrap_object)) return -1;
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)

	self->wrapped = IRStmt_WrTmp(tmp, data->wrapped);
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtWrTmp, IRStmt, wrapped->Ist.WrTmp.tmp, tmp, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtWrTmp, IRStmt, wrapped->Ist.WrTmp.data, data, IRExpr)

static PyGetSetDef pyIRStmtWrTmp_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtWrTmp, tmp),
	PYVEX_ACCESSOR_DEF(IRStmtWrTmp, data),
	{NULL}
};

static PyMethodDef pyIRStmtWrTmp_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRStmtWrTmp, IRStmt);
