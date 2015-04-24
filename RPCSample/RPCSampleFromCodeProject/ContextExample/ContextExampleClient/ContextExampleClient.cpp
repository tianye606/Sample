// File ContextExampleClient.cpp
#include <iostream>
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
      LPSTR(&pBuffer),
      0,
      NULL);

   std::cerr << szFunction << " failed. "
      << (pBuffer ? LPCSTR(pBuffer) : "Unknown error. ")
      << "(" << dwError << ")" << std::endl;
   LocalFree(pBuffer);
   return dwError;
}

int main()
{
   RPC_STATUS status;
   unsigned char* szStringBinding = NULL;

   std::clog << "Calling RpcStringBindingCompose" << std::endl;
   // Creates a string binding handle.
   // This function is nothing more than a printf.
   // Connection is not done here.
   status = RpcStringBindingCompose(
      NULL, // UUID to bind to.
      reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP protocol.
      reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network address to use.
      reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
      NULL, // Protocol dependent network options to use.
      &szStringBinding); // String binding output.
   if (status)
      return HandleError("RpcStringBindingCompose", status);

   handle_t hBinding = NULL;

   std::clog << "Calling RpcBindingFromStringBinding" << std::endl;
   // Validates the format of the string binding handle and converts
   // it to a binding handle.
   // Connection is not done here either.
   status = RpcBindingFromStringBinding(
      szStringBinding, // The string binding to validate.
      &hBinding); // Put the result in the explicit binding handle.
   if (status)
      return HandleError("RpcBindingFromStringBinding", status);

   std::clog << "Calling RpcStringFree" << std::endl;
   // Free the memory allocated by a string.
   status = RpcStringFree(
      &szStringBinding); // String to be freed.
   if (status)
      return HandleError("RpcStringFree", status);

   std::clog << "Calling RpcEpResolveBinding" << std::endl;
   // Resolves a partially-bound server binding handle into a
   // fully-bound server binding handle.
   status = RpcEpResolveBinding(hBinding, ContextExample_v1_0_c_ifspec);
   if (status)
      return HandleError("RpcEpResolveBinding", status);

   RpcTryExcept
   {
      std::clog << "Calling Open" << std::endl;
      // Open the context handle.
      CONTEXT_HANDLE hContext = Open(hBinding, "Hello Context World!");

      std::cout << "Press enter to call Output" << std::endl;
      std::cin.get();

      std::clog << "Calling Output" << std::endl;
      // Calls the RPC function. The hBinding binding handle
      // is used explicitly.
      Output(hContext);

      std::cout << "Press enter to call Close" << std::endl;
      std::cin.get();

      std::clog << "Calling Close" << std::endl;
      // Close the context handle.
      Close(&hContext);
   }
   RpcExcept(1)
   {
      HandleError("Remote Procedure Call", RpcExceptionCode());
   }
   RpcEndExcept

   std::clog << "Calling RpcBindingFree" << std::endl;
   // Releases binding handle resources and disconnects from the server.
   status = RpcBindingFree(
      &hBinding); // Frees the explicit binding handle.
   if (status)
      return HandleError("RpcBindingFree", status);

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
