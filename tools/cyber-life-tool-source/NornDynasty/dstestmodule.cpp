// $Id: dstestmodule.cpp,v 1.10 2001/01/22 18:25:09 firving Exp $

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <Python.h>

#include "NornDynasty.h"
#include "DSStress.h"

static PyObject *dstest_hostport(PyObject *, PyObject *args)
{
    char* server;
	int port;
    if (!PyArg_ParseTuple(args, "si", &server, &port))
        return NULL;

	DSNetManager::OverrideHost(server, port);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *dstest_new(PyObject *, PyObject *args)
{
    char *username;
	char *password;
    if (!PyArg_ParseTuple(args, "ss", &username, &password))
        return NULL;

	DSStress* stress = new DSStress(username, password);
	PyObject* netPy = PyCObject_FromVoidPtr((void *)stress, NULL);

    return Py_BuildValue("O", netPy);
}

static PyObject *dstest_delete(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		delete stress;
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_onlineoffline(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		stress->OnlineOffline();
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_addworld(PyObject *, PyObject *args)
{
    PyObject *meo;
	int initialNorns;
    if (!PyArg_ParseTuple(args, "Oi", &meo, &initialNorns))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		stress->NewDynastyWorld(initialNorns);
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_uploadhistory(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		stress->UploadNewHistory();
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_thisuser(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		std::string user = stress->GetNet().GetUser();

		return PyString_FromString(user.c_str());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_randomuser(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		std::string user = stress->FetchRandomUser();

		return PyString_FromString(user.c_str());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_usernickname(PyObject *, PyObject *args)
{
    PyObject *meo;
	char *uin;
    if (!PyArg_ParseTuple(args, "Os", &meo, &uin))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		std::string nickName;

		bool block = true;
		while (block)
		{
			stress->GetNet().FetchUserData(uin, nickName, block);
			Sleep(1);
		}

		return PyString_FromString(nickName.c_str());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}



static PyObject *dstest_bytessent(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		return PyInt_FromLong(stress->BytesSentEver());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_bytesreceived(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		return PyInt_FromLong(stress->BytesReceivedEver());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_warpset(PyObject *, PyObject *args)
{
    PyObject *meo;
	char *inbox, *outbox, *extension;
    if (!PyArg_ParseTuple(args, "Osss", &meo, &inbox, &outbox, &extension))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);

		stress->GetNet().SetInboxDirectory(inbox);
		stress->GetNet().SetOutboxDirectory(outbox);
		stress->GetNet().SetWarpFileExtension(extension);
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *dstest_warpmakeoutfilename(PyObject *, PyObject *args)
{
    PyObject *meo;
	char* destinationUser;
    if (!PyArg_ParseTuple(args, "Os", &meo, &destinationUser))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);

		std::string name = stress->GetNet().ReturnUniqueOutboxFilename(destinationUser);
		return PyString_FromString(name.c_str());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_warpuserwhosentfile(PyObject *, PyObject *args)
{
    PyObject *meo;
	char* filename;
    if (!PyArg_ParseTuple(args, "Os", &meo, &filename))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);

		std::string user = stress->GetNet().FindUserWhoSentFile(filename);
		return PyString_FromString(user.c_str());
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_warpsendnow(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		stress->GetNet().SendOrdinaryMessages();
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_warpspoolmessages(PyObject *, PyObject *args)
{
    PyObject *meo;
    if (!PyArg_ParseTuple(args, "O", &meo))
        return NULL;

	if (PyCObject_Check(meo))
	{
		DSStress* stress = (DSStress*)PyCObject_AsVoidPtr(meo);
		stress->GetNet().SpoolMessages();
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_setdynastyparams(PyObject *, PyObject *args)
{
    PyObject *params;
    if (!PyArg_ParseTuple(args, "O", &params))
        return NULL;

	if (PyDict_Check(params))
	{
		NornDynasty::ourLifeStageLength = PyInt_AsLong(PyDict_GetItemString(params, "LifeStageLength"));
		NornDynasty::ourBreedGoes = PyFloat_AsDouble(PyDict_GetItemString(params, "BreedGoes"));

		NornDynasty::ourChanceOfSplice = PyInt_AsLong(PyDict_GetItemString(params, "CanceOfSplice"));

		NornDynasty::ourDeathGoes = PyFloat_AsDouble(PyDict_GetItemString(params, "DeathGoes"));
		NornDynasty::ourCloneGoes = PyFloat_AsDouble(PyDict_GetItemString(params, "CloneGoes"));
		NornDynasty::ourExportGoes = PyFloat_AsDouble(PyDict_GetItemString(params, "ExportGoes"));
		NornDynasty::ourImportGoes = PyFloat_AsDouble(PyDict_GetItemString(params, "ImportGoes"));
		NornDynasty::ourChanceDuplicateEvent = PyInt_AsLong(PyDict_GetItemString(params, "ChanceDuplicateEvent"));
    }
	else
	{
		// TODO: Exception
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *dstest_updatedynasty(PyObject *, PyObject *args)
{
	NornDynasty::UpdateDynasty();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*dstest_dynastystats(PyObject*, PyObject *args)
{
	PyObject* dict = PyDict_New();
	if (!dict)
	{
		// TODO: Exception
	}

	PyDict_SetItemString(dict, "EventSentCount", PyInt_FromLong(NornDynasty::GetEventCount()));	
	PyDict_SetItemString(dict, "AliveNornCount", PyInt_FromLong(NornDynasty::GetNornCount()));	
	PyDict_SetItemString(dict, "MotherDiedWhileInWombCount", PyInt_FromLong(NornDynasty::GetMotherDiedWhileInWombCount()));
	
	return dict;
}

static PyMethodDef DSTestMethods[] =
{
    {"hostport",  dstest_hostport, METH_VARARGS},

	// DSStress object
    {"new",  dstest_new, METH_VARARGS},
	{"delete", dstest_delete, METH_VARARGS},
    {"onlineoffline",  dstest_onlineoffline, METH_VARARGS},
    {"addworld",  dstest_addworld, METH_VARARGS},
    {"uploadhistory",  dstest_uploadhistory, METH_VARARGS},
    {"randomuser",  dstest_randomuser, METH_VARARGS},
    {"thisuser",  dstest_thisuser, METH_VARARGS},
	{"usernickname", dstest_usernickname, METH_VARARGS},
	// Statistics
    {"bytessent",  dstest_bytessent, METH_VARARGS},
    {"bytesreceived",  dstest_bytesreceived, METH_VARARGS},
	// Warping
    {"warpset",  dstest_warpset, METH_VARARGS},
    {"warpmakeoutfilename",  dstest_warpmakeoutfilename, METH_VARARGS},
	{"warpuserwhosentfile", dstest_warpuserwhosentfile, METH_VARARGS},
	{"warpsendnow", dstest_warpsendnow, METH_VARARGS},
	{"warpspoolmessages", dstest_warpspoolmessages, METH_VARARGS},

	// Dynasty
    {"setdynastyparams",  dstest_setdynastyparams, METH_VARARGS},
    {"updatedynasty",  dstest_updatedynasty, METH_VARARGS},
	// Statistics
	{"dynastystats", dstest_dynastystats, METH_VARARGS},


    {NULL,      NULL}        /* Sentinel */
};

void initdstest()
{
    (void) Py_InitModule("dstest", DSTestMethods);
}

