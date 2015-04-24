// File Example1ExplicitClient.cpp
#include <iostream>
#include "../Example1Explicit/Example1Explicit.h"

int main()
{
   RPC_STATUS status;
   unsigned char* szStringBinding = NULL;

   // Creates a string binding handle.
   // This function is nothing more than a printf.
   // Connection is not done here.
   status = RpcStringBindingCompose(
      NULL, // UUID to bind to.
      reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP
                                                        // protocol.
      reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network
                                                     // address to use.
      reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
      NULL, // Protocol dependent network options to use.
      &szStringBinding); // String binding output.

   if (status)
      exit(status);

   handle_t hExample1ExplicitBinding = NULL;

   // Validates the format of the string binding handle and converts
   // it to a binding handle.
   // Connection is not done here either.
   status = RpcBindingFromStringBinding(
      szStringBinding, // The string binding to validate.
      &hExample1ExplicitBinding); // Put the result in the explicit
                                  // binding handle.

   if (status)
      exit(status);

   RpcTryExcept
   {
      // Calls the RPC function. The hExample1ExplicitBinding binding handle
      // is used explicitly.
      // Connection is done here.
      Output(hExample1ExplicitBinding, "Hello Explicit RPC World!");
   }
   RpcExcept(1)
   {
      std::cerr << "Runtime reported exception " << RpcExceptionCode()
                << std::endl;
   }
   RpcEndExcept

   // Free the memory allocated by a string.
   status = RpcStringFree(
      &szStringBinding); // String to be freed.

   if (status)
      exit(status);

   // Releases binding handle resources and disconnects from the server.
   status = RpcBindingFree(
      &hExample1ExplicitBinding); // Frees the binding handle.

   if (status)
      exit(status);
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
