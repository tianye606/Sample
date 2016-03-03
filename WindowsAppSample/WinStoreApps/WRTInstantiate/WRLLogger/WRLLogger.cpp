// WRLLogger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wrl\implements.h>
#include <comutil.h>
#include <Shlwapi.h>

#include "ILogger_h.h"

using namespace Microsoft::WRL;

// Writes logging messages to the console.
class CConsoleWriter : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ILogger>
{
public:
    CConsoleWriter()
    {
        m_category = nullptr;
    }
    HRESULT RuntimeClassInitialize(_In_ PCWSTR category)
    {
        return SHStrDup(category, &m_category);
    }

    STDMETHODIMP Log(_In_ PCWSTR text)
    {
        if (m_category != nullptr)
            wprintf_s(L"%s: %s\n", m_category, text);
        else
            wprintf_s(L"%s\n", text);
        return S_OK;
    }

private:
    PWSTR m_category;
    // Make destroyable only through Release.
    ~CConsoleWriter()
    {
        CoTaskMemFree(m_category);
    }
};

void incr(int&& rr) { rr++; }

int _tmain(int argc, _TCHAR* argv[])
{
    ComPtr<CConsoleWriter> writer;
    HRESULT hr;
    //To handle construction failure for the basic logger component
    hr = MakeAndInitialize<CConsoleWriter>(&writer, L"Info");

    //To create and instantiate a basic logger component without construction failure
    //writer = Make<CConsoleWriter>();
    hr = writer->Log(L"Logger ready.");

    //rvalue reference
    double t = 1;
    incr(1);
    return hr;
}

