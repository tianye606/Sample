//TODO::Need enable WPP preprocessor

#define WPP_CONTROL_GUIDS                                              \
    WPP_DEFINE_CONTROL_GUID(                                           \
        myDriverTraceGuid, (84bdb2e9,829e,41b3,b891,02f454bc2bd7), \
        WPP_DEFINE_BIT(MYDRIVER_ALL_INFO)        /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(TRACE_DRIVER)             /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(TRACE_DEVICE)             /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(TRACE_QUEUE)              /* bit  3 = 0x00000008 */ \
        )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags)                              \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags)                            \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// begin_wpp config
// USEPREFIX (TraceEntry, "%!STDPREFIX!");
// FUNC TraceEntry{LEVEL=TRACE_LEVEL_VERBOSE}(...);
// USESUFFIX (TraceEntry, "--> %!FUNC!");
// end_wpp


// begin_wpp config
// USEPREFIX (TraceExit, "%!STDPREFIX!");
// FUNC TraceExit{LEVEL=TRACE_LEVEL_VERBOSE}(EXP);
// USESUFFIX (TraceExit, "<-- %!FUNC!: 0x%x", EXP);
// end_wpp

//
// This comment block is scanned by the trace preprocessor to define the 
// TraceEvents function.
//
// begin_wpp config
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//
