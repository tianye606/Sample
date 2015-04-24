// File ContextExampleServer.cpp
#include <iostream>
#include <string>
#include "../ContextExample/ContextExample.h"

// Write a formatted error message to std::cerr.
DWORD HandleError(const char* szFunction, DWORD dwError)
{
   void* pBuffer = NULL;
   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
      NULL,
      dwError,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      LPTSTR(&pBuffer),
      0,
      NULL);

   std::cerr << szFunction << " failed. "
      << (pBuffer ? LPCTSTR(pBuffer) : "Unknown error. ")
      << "(" << dwError << ")" << std::endl;
   LocalFree(pBuffer);
   return dwError;
}

CONTEXT_HANDLE Open(
   /* [in] */ handle_t hBinding,
   /* [string][in] */ const char* szString)
{
   std::string* pContext = new std::string(szString);
   CONTEXT_HANDLE hContext = pContext;
   std::clog << "Open: Binding = " << hBinding
      << "; Context = " << hContext << std::endl;
   return hContext;
}

void Output(
   /* [in] */ CONTEXT_HANDLE hContext)
{
   std::clog << "Output: Context = " << hContext << std::endl;
   std::string* pContext = static_cast<std::string*>(hContext);
   std::cout << *pContext << std::endl;
}

void Close(
   /* [out][in] */ CONTEXT_HANDLE* phContext)
{
   std::clog << "Close: Context = " << *phContext << std::endl;
   std::string* pContext = static_cast<std::string*>(*phContext);
   delete pContext;

   // We must set the context handle to NULL, or else we will get
   // a rundown later anyway.
   *phContext = NULL;
}

// The RPC runtime will call this function if the connection to the client
// is lost.
void __RPC_USER CONTEXT_HANDLE_rundown(CONTEXT_HANDLE hContext)
{
   std::clog << "CONTEXT_HANDLE_rundown: Context = " << hContext << std::endl;
   Close(&hContext);
}

// The thread that will listen for incoming RPC calls.
DWORD WINAPI RpcServerListenThreadProc(LPVOID /*pParam*/)
{
   // Start to listen for remote procedure calls for all registered interfaces.
   // This call will not return until RpcMgmtStopServerListening is called.
   return RpcServerListen(
      1, // Recommended minimum number of threads.
      RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Recommended maximum number of threads.
      FALSE); // Start listening now.
}

// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}

int main()
{
   RPC_STATUS status;

   std::clog << "Calling RpcServerUseProtseqEp" << std::endl;
   // Uses the protocol combined with the endpoint for receiving
   // remote procedure calls.
   status = RpcServerUseProtseqEp(
      reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP protocol.
      RPC_C_PROTSEQ_MAX_REQS_DEFAULT, // Backlog queue length for TCP/IP.
      reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
      NULL); // No security.
   if (status)
      return HandleError("RpcServerUseProtseqEp", status);

   std::clog << "Calling RpcServerRegisterIf" << std::endl;
   // Registers the ContextExample interface.
   status = RpcServerRegisterIf2(
      ContextExample_v1_0_s_ifspec, // Interface to register.
      NULL, // Use the MIDL generated entry-point vector.
      NULL, // Use the MIDL generated entry-point vector.
      RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
      RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
      (unsigned)-1, // Infinite max size of incoming data blocks.
      SecurityCallback); // Naive security callback.
   if (status)
      return HandleError("RpcServerRegisterIf", status);

   std::clog << "Creating listen thread" << std::endl;
   const HANDLE hThread = CreateThread(NULL, 0, RpcServerListenThreadProc,
      NULL, 0, NULL);
   if (!hThread)
      return HandleError("CreateThread", GetLastError());

   std::cout << "Press enter to stop listening" << std::endl;
   std::cin.get();

   std::clog << "Calling RpcMgmtStopServerListening" << std::endl;
   status = RpcMgmtStopServerListening(NULL);
   if (status)
      return HandleError("RpcMgmtStopServerListening", status);

   std::clog << "Waiting for listen thread to finish";
   while (WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT)
      std::clog << '.';
   std::clog << std::endl << "Listen thread finished" << std::endl;

   DWORD dwExitCodeThread = 0;
   GetExitCodeThread(hThread, &dwExitCodeThread);
   CloseHandle(hThread);
   if (dwExitCodeThread)
      return HandleError("RpcServerListen", dwExitCodeThread);

   std::cout << "Press enter to exit" << std::endl;
   std::cin.get();
}

// Memory allocation function for RPC.
// The runtime uses these two functions for allocating/deallocating
// enough memory to pass the string to the server.
void* __RPC_USER midl_user_allocate(size_t size)
{
   return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(void* p)
{
   free(p);
}
