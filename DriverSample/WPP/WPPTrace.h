//TODO::Need enable WPP preprocessor

#define WPP_CONTROL_GUIDS                                              \
    WPP_DEFINE_CONTROL_GUID(                                           \
        myDriverTraceGuid, (84bdb2e9,829e,41b3,b891,02f454bc2bd7), \
        WPP_DEFINE_BIT(WPP_BIT_ALL_INFO)        /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(WPP_BIT_TRACE_DRIVER)             /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(WPP_BIT_TRACE_DEVICE)             /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(WPP_BIT_TRACE_QUEUE)              /* bit  3 = 0x00000008 */ \
        )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags)                              \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags)                            \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// begin_wpp config
// USEPREFIX (TraceEntry, "%!STDPREFIX!");
// FUNC TraceEntry{LEVEL=TRACE_LEVEL_VERBOSE, FLAGS=WPP_BIT_ALL_INFO}(...);
// USESUFFIX (TraceEntry, "--> %!FUNC!");
// end_wpp

//begin_wpp config
//USEPREFIX (TRACE_RETURN, "%!STDPREFIX!");
//FUNC TRACE_RETURN{LEVEL=TRACE_LEVEL_VERBOSE, FLAGS=WPP_BIT_ALL_INFO}(EXP);
//USESUFFIX (TRACE_RETURN, "<-- %!FUNC!:Function Return=0x%x",EXP);
//end_wpp

// WPP preprocessor will call WPP_Param1_Param2_Param3_ENABLED and WPP_Param1_Param2_Param3_LOGGER in tmh
// Here, Param1 is LEVEL, Param1 is FLAGS, Param3 is EXP, so we need define them here. 
// They are for checking if the event should be logged, and for
// choosing the logger handle to use when calling the ETW trace API.
// If we have Param4, we need include Param4 in the MACRO also
#define WPP_LEVEL_FLAGS_EXP_ENABLED(lvl,flags, HR) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)
#define WPP_LEVEL_FLAGS_EXP_LOGGER(lvl,flags, HR) WPP_LEVEL_LOGGER(flags)

//
// This comment block is scanned by the trace preprocessor to define the 
// TraceEvents function.
//
// begin_wpp config
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//
